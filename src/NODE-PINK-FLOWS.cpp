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
#include<H4P_WiFi.h>

//======================================================================================================================================
//
//      AnalogAverage
//
h4pAnalogAverage::h4pAnalogAverage(uint8_t p,uint32_t f,uint32_t n,npNODE* d):
    h4pGPIO(p,INPUT,ACTIVE_LOW,{
        npPassTimer, 
        new npAVERAGE{n},
        d
    }){
        H4P_PinMachine::periodicRead(p,f);
}
//
//      AnalogAvgChanged
//
h4pAnalogAvgChanged::h4pAnalogAvgChanged(uint8_t p,uint32_t f,uint32_t n,uint32_t h,npNODE* d):
    h4pGPIO(p,INPUT,ACTIVE_LOW,{ 
        npPassTimer, 
        new npAVERAGE{n},
        new npVALUEDIFF{h},
        d
    }){ 
        H4P_PinMachine::periodicRead(p,f);
}
//
//      AnalogAvgRolling
//
h4pAnalogAvgRolling::h4pAnalogAvgRolling(uint8_t p,uint32_t f,npNODE* d):
    h4pGPIO(p,INPUT,ACTIVE_LOW,{ 
        npPassTimer, 
        new npROLLINGAVERAGE,
        d
    }){ 
        H4P_PinMachine::periodicRead(p,f);
}
//
//      AnalogAvgWindow
//
h4pAnalogAvgWindow::h4pAnalogAvgWindow(uint8_t p,uint32_t f,uint32_t w,npNODE* d):
    h4pGPIO(p,INPUT,ACTIVE_LOW,{ 
        npPassTimer, 
        new npAVGWINDOW{w},
        d
    }){ 
        H4P_PinMachine::periodicRead(p,f);
}
//
//      AnalogPolled
//
h4pAnalogPolled::h4pAnalogPolled(uint8_t p,uint32_t f,uint32_t h,npNODE* d):
    h4pGPIO(p,INPUT,ACTIVE_LOW,{ 
        npPassTimer, 
        new npVALUEDIFF{h},
        d
    }){
        H4P_PinMachine::periodicRead(p,f);
}
//
//      ANALOG THRESHOLD
//
h4pAnalogThreshold::h4pAnalogThreshold(uint8_t p,uint32_t f,H4PM_COMPARE cmp,uint32_t lim,uint32_t h,npNODE* d):
    h4pGPIO(p,INPUT,ACTIVE_LOW,{
        npPassTimer, 
        new npVALUEDIFF{h},
        new npTHRESHOLD{lim,cmp},
        d
    }){ 
        H4P_PinMachine::periodicRead(p,f);
}
//
//      AnalogTMP36
//
h4pAnalogTMP36::h4pAnalogTMP36(uint8_t p,uint32_t f,uint32_t h,npNODE* d):
    h4pGPIO(p,INPUT,ACTIVE_LOW,{ 
        npPassTimer, 
        new npVALUEDIFF{h},
        new npANALOGTMP36,
        d
    }){
        H4P_PinMachine::periodicRead(p,f);
}
//
//      CIRCULAR
//
h4pCircular::h4pCircular(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t dbTime,uint32_t N):
    h4pGPIO(p,m,s,{
        new npSMOOTH{dbTime}, // common thsi out?
        npFilterINACTIVE,
        new npPUBLISHCYCLE{N}, // use sigmaE as value, i.e. counts incresing number of up/down click pairs
    }){
}
//
//      COUNTING
//
h4pCounting::h4pCounting(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t dbTime):
    h4pGPIO(p,m,s,{
        new npSMOOTH{dbTime},
        npFilterINACTIVE,
        new npPUBLISHSIGE
    }){

}

//
//      DEBOUNCED
//
h4pDebounced::h4pDebounced(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t dbTime,npNODE* d):
    h4pGPIO(p,m,s,{
        new npSMOOTH{dbTime},
        d
    }){
}
//
//      ENCODERS
//
// HALF
h4pEncoder_Half::h4pEncoder_Half(uint8_t p,uint8_t m,H4PM_SENSE s,npFLOW f): h4pGPIO(p,m,s,f){}

void h4pEncoder_Half::_announce(){ _prev={_p,0,0,_normalise(_r),_r,false,false,1}; }
// FULL
h4pEncoder::h4pEncoder(uint8_t pA,uint8_t pB,uint8_t m,H4PM_SENSE s,bool sord,npNODE* d){ // Single OR Dual
    new h4pEncoder_Half(pA,m,s,{
        new npROTARYDECODE{pA,pB,sord},
        d
    });
    new h4pEncoder_Half(pB,m,s,{ 
        new npPASSTHRU{pA}
    });
}

h4pEncoderAuto::h4pEncoderAuto(uint8_t pA,uint8_t pB,uint8_t m,H4PM_SENSE s,int vMin,int vMax,int vInc,int vSet,bool wrap,npNODE* d){
    _pA=new h4pEncoder_Half(pA,m,s,{
            new npROTARYDECODE{pA,pB,true}, // single = +/- 1 out on pinA, dual = +1 on pinA / pinB
            new npLIMITER{this,vMin,vMax,vInc,vSet,wrap},
            new npVALUEDIFF,
            d
    });
    new h4pEncoder_Half(pB,m,s,{ 
        new npPASSTHRU{pA}
    });
}
//
//      FILTERED
//
h4pFiltered::h4pFiltered(uint8_t p,uint8_t m,H4PM_SENSE s,uint8_t filter,npNODE* d):
    h4pGPIO(p,m,s,{
        new npFILTER{filter},
        d
    }){
}
//
//      LATCHING
//
h4pLatching::h4pLatching(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t t,npNODE* d):
    h4pGPIO(p,m,s,{
        new npSMOOTH{t},
        npFilterINACTIVE,
        new npFLIPFLOP, // use sigmaE as value, i.e. counts incresing number of up/down click pairs
        d
    }){
}
//
//      Multistage
//
h4pMultistage::h4pMultistage(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t t,H4PM_STAGE_MAP sm,npNODE* d):
    h4pGPIO(p,m,s,{
        new npSMOOTH{t},
        new npDELTAREPEAT{H4PM_GRANULARITY}, // hoist
        new npSTAGEMANAGER{sm},
        d
    }){
}
//
//      MFNB
//
void h4pMultifunctionButton::_handleEvent(const std::string& s,H4PE_TYPE t,const std::string& m){
    if(STOI(s)==_p){
        switch(STOI(m)){
            case -2:
                YEVENT(H4PE_SIGNAL,"10,.. ");
                break;
            case -1:
                YEVENT(H4PE_SIGNAL,"25,. ");
                break;
            case 0:
                h4p.gvSetInt(stateTag(),!h4p.gvGetInt(stateTag())); 
                break;
            case 1:
                QEVENT(H4PE_REBOOT);
            case 2:
                QEVENT(H4PE_FACTORY);
        }
    }
}

h4pMultifunctionButton::h4pMultifunctionButton(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t t): 
    h4pMultistage(p,m,s,t,{H4MF_REBOOT,H4MF_FACTORY}){
        h4pregisterhandler(_me,H4PE_GPIO,[=](const std::string& s,H4PE_TYPE t,const std::string& m){ _handleEvent(s,t,m); });
}
//
//      OUTPUT
//
void h4pOutput::turn(bool b){ digitalWrite(_p,_normalise(b)); }

h4pOutput::h4pOutput(uint8_t p,H4PM_SENSE s,uint8_t i,uint8_t c,npNODE* d): h4pGPIO(p,OUTPUT,s,{d}){ _c=c; turn(i); }
//
//      POLLED
//
h4pPolled::h4pPolled(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t f,npNODE* d): 
    h4pGPIO(p,m,s,{
        npPassTimer, 
        new npVALUEDIFF,
        d
    }){
    H4P_PinMachine::periodicRead(p,f);
}
//
//      RAW
//
h4pRaw::h4pRaw(uint8_t p,uint8_t m,H4PM_SENSE s,npNODE* d): h4pGPIO(p,m,s,{ d }){}
//
//      REPEATING
//
h4pRepeating::h4pRepeating(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t dbTime,uint32_t f):
    h4pGPIO(p,m,s,{
        new npSMOOTH{dbTime},
        new npDELTAREPEAT{f},
        npPublishValue
    }){
}
//
//      RETRIGGERING
//
h4pRetriggering::h4pRetriggering(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t t,npNODE* d): 
    h4pGPIO(p,m,s,{
        new npTRIGGER{t},
        d
    }){
}

void h4pRetriggering::_announce(){
    if(logicalRead()) inject(_r,false); // synthesise "missing" trigger event from T < boot time: keeps us in sync
    else h4pGPIO::_announce();
}
//
//      TACTLESS
//
h4pTactless::h4pTactless(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t dbTime,npNODE* d):
    h4pGPIO(p,m,s,{
        new npSMOOTH{dbTime},
        npFilterINACTIVE,
        d
    }){
}
//
//      TIMED
//
h4pTimed::h4pTimed(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t dbTime): 
    h4pGPIO(p,m,s,{
        new npSMOOTH{dbTime},
        new npPUBLISHDELTA
    }){
}