/*
 MIT License

Copyright (c) 2020 Phil Bowles <H48266@gmail.com>
   github     https://github.com/philbowles/H4
   blog       https://8266iot.blogspot.com
   groups     https://www.facebook.com/groups/esp8266questions/
              https://www.facebook.com/H4-Esp8266-Firmware-Support-2338535503093896/


Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
#include<H4P_PinMachine.h>

npFILTER*        npFilterINACTIVE = new npFILTER{OFF};   // passes thru only INACTIVE transitions ie 0->1 if ACTIVE_LOW, or 1->0 if ACTIVE_HIGH
npPASSTIMER*     npPassTimer = new npPASSTIMER;          // allows only timer events for polled strategies 
npPUBLISHVALUE*  npPublishValue = new npPUBLISHVALUE;    // emits GPIO event from the pin with event msg = value
npSHOW*          npShow = new npSHOW;                    // debug dump of all fields in msg    

msg npNODE::publish(msg m,int v,H4PE_TYPE t){
    m.sigE++;
    h4psysevent(stringFromInt(m.p,"%02d"),t,"%d",m.load=v);
    return m;
}
//==============================================================================================================================================
//
//      DELTAREPEAT
//
msg npDELTAREPEAT::operator()(msg m) {
    if(_rpt){
        if(m.phy!=_kravitz){
            h4.cancel(_rpt);
            _rpt=nullptr;
            m.timer=false;
        }
        m.load=m.time-T;
    } 
    else {
        _kravitz=m.phy;
        T=m.time;
        _rpt=h4.every(_f,[=](){ H4P_PinMachine::inject(m.p,_kravitz,true); },nullptr,H4P_TRID_RPTP);
        m.halt=true;
    }
    return m;
};
//
//      LIMITER
//
int npLIMITER::_clip(int v){
    int tmp=_value+(v * _vInc);
    if(_wrap){
        if(tmp < _vMin) return _vMax;
        else return tmp > _vMax ? _vMin:tmp;
    } else return constrain(tmp,_vMin,_vMax);
}

void npLIMITER::_set(int v){
    _value=v;
    auto pp=_owner->_pA;
    if(pp)(*pp->_pipeline.back())({pp->_p,micros(),0,v,0,false,false,pp->_prev.sigE});
}
//
//      PASSTHRU
//
msg npPASSTHRU::operator()(msg m){ return H4P_PinMachine::runFlow(_pA,m); }
//
//
//
msg npPUBLISHONOFF::operator()(msg m) { h4p[stateTag()]=m.load; return m; }//return publish(m,m.load,H4PE_STATE); }
//
//      ROTARYDECODE
//

//  if single == false, then you get a +1 event on pinB per clockwise click or +1 on pinA per anticlockwise click
//  if single == true you will get NOTHING out pinB and +1 out of PinA for clockwise, -1 for anticlockwise
//  SO if you turn 3 clicks one way then three clicks back the other way:
//
//  single == false:
//      PIN A 1 1 1
//      PIN B       1 1 1
//
//  single == true:
//      PIN A 1 1 1 -1 -1 -1
//      PIN B                  pB will NEVER emit events in "single" mode: in fact, THAT'S THE POINT of "single" mode!
//      
//      aggregating +/- onto a single pin allows for extreme flexibility when continuing the pipeline.
//
//#include<bitset>
msg npROTARYDECODE::operator()(msg m){
    uint8_t a=digitalRead(_pA);
    uint8_t b=digitalRead(_pB);
    _AB <<= 2;
    _AB |= (((a << 1) | b) & 0x03);
    _val+=_roti[_AB & 0x0f];
    if(_val== 4 || _val == -4){ // feels all kindsa wrong
        m.load=_val/4;
        _val=0;
        if(_sord)  m.p=_pA; // everything out of pA, antis stay -ve
        else if(m.load < 0) m.load=1; // antis become +ve out on the other pin
        m.halt=false;
    } else m.halt=true;
    return m;
}
//
//      SMOOTH
//
void npSMOOTH::_restartTimer(uint8_t p){ h4.cancel(T); T=_startTimer(p); }

H4_TIMER npSMOOTH::_startTimer(uint8_t p){ 
    return h4.once(_ms,[=](){
        _stopTimer();
        H4P_PinMachine::inject(p,digitalRead(p),true);
    },nullptr,H4P_TRID_DBNC); 
}

void npSMOOTH::_stopTimer(){ h4.cancel(T); T=nullptr; }

msg npSMOOTH::operator()(msg m){
    if(!m.timer){
        if(T){
            if(_kravitz==m.phy) _restartTimer(m.p);
            else _stopTimer();
        }
        else {
            _kravitz=m.phy;
            T=_startTimer(m.p);
        }
        m.halt=true;
    }
    return m;
}
//
//      STAGE MANAGER
//
msg npSTAGEMANAGER::operator()(msg m) {
    if(m.timer){
        m.halt=true;
        if(_stage < _sm.size()){
            auto div1k=m.load/1000;
            if(div1k > _sm[_stage]){
                m.load=-1*(1+_stage++);
                m.halt=false;
            }
        }
    } 
    else {
        m.load=_stage;
        _stage=0;
    }
    return m;
}
//
//      TRIGGER
//
msg npTRIGGER::operator()(msg m){
    if(!m.timer){
        if(T){
            m.halt=true;
            if(_kravitz==m.phy) _restartTimer(m.p);
        }
        else {
            T=_startTimer(m.p);
            _kravitz=m.phy;
        }
    }
    return m;
}
//
//      UPDATE INTEGER
//
msg npUPDATEGLOBAL::operator()(msg m){
    h4p[_gv]=m.load;
    return m;
}
//
//      UPDATE INTEGER
//
msg npUPDATEINT::operator()(msg m){
    _i=m.load;
    return m;
}