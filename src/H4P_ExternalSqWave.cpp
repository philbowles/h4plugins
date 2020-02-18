/*
 MIT License

Copyright (c) 2019 Phil Bowles <h4plugins@gmail.com>
   github     https://github.com/philbowles/esparto
   blog       https://8266iot.blogspot.com     
   groups     https://www.facebook.com/groups/esp8266questions/
              https://www.facebook.com/Esparto-Esp8266-Firmware-Support-2338535503093896/
                			  

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
#include<H4P_ExternalSqWave.h>

void  H4P_ExternalSqWave::_Sweep(uint32_t c,uint32_t lim,uint32_t p,uint32_t l,uint32_t h,uint32_t i,function<void(uint32_t)> onChange){
    h4.every(p,bind([this](uint32_t c,uint32_t l,uint32_t h,uint32_t i,function<void(uint32_t)> f){
        pr<uint32_t> value=l;
        if(value<h+i){
            f(value);
            __send(c,value);
            value+=i;
        } else h4.cancel();
    },c,l,constrain(h,1,lim),constrain(i,1,lim),onChange),nullptr,H4P_TRID_SQWV,true);
}

void H4P_ExternalSqWave::__send(uint32_t c,uint32_t v){
    string sv;
    sv.push_back((char)c);
    sv.append(stringFromInt(v));
    print(CSTR(sv));
    delay(H4ESW_TIMEOUT);    
}

uint32_t H4P_ExternalSqWave::__set(vector<string> vs,uint32_t c,uint32_t lim){
    return guardInt1(vs,bind([c,lim,this](uint32_t v){
        __send(c,constrain(v,1,lim)); 
        return H4_CMD_OK;
    },_1));
}

uint32_t H4P_ExternalSqWave::__sweep(vector<string> vs,uint32_t c,uint32_t lim){
    return guardInt4(vs,bind([c,lim,this](uint32_t p,uint32_t l,uint32_t h,uint32_t i){ _Sweep(c,lim,p,l,h,i,_default);  },_1,_2,_3,_4));
} 
//
//      cmd responders
//
uint32_t H4P_ExternalSqWave::_fset(vector<string> vs){ return(__set(vs,'F',H4ESW_MAX_F)); }

uint32_t H4P_ExternalSqWave::_dset(vector<string> vs){ return(__set(vs,'D',H4ESW_MAX_D)); }

uint32_t H4P_ExternalSqWave::_fsweep(vector<string> vs){ return __sweep(vs,'F',H4ESW_MAX_F); }

uint32_t H4P_ExternalSqWave::_dsweep(vector<string> vs){ return __sweep(vs,'D',H4ESW_MAX_D); }
//
//      H4P_ExternalSqWave
//
H4P_ExternalSqWave::H4P_ExternalSqWave(uint8_t rx,uint8_t tx,uint32_t initialF,uint32_t initialD): SoftwareSerial(rx,tx){ 
    _pid=esqwTag();
    _names={ {H4P_TRID_SQWV,uppercase(_pid)} };
    uint32_t H4PC_ESW_SET=++nextSubid;
    uint32_t H4PC_ESW_SWEEP=++nextSubid;
    _cmds={
        {_pid,      { H4PC_ROOT, subid, nullptr}},
        {"set",     { subid, H4PC_ESW_SET, nullptr}},
        {"d",       { H4PC_ESW_SET, 0, CMDVS(_dset)}},
        {"f",       { H4PC_ESW_SET, 0, CMDVS(_fset)}},
        {"stop",    { subid, 0, CMD(stop)}},            
        {"sweep",   { subid, H4PC_ESW_SWEEP, nullptr}},
        {"f",       { H4PC_ESW_SWEEP, 0, CMDVS(_fsweep)}},
        {"d",       { H4PC_ESW_SWEEP, 0, CMDVS(_dsweep)}}
    };
    h4.queueFunction([initialF,initialD,this](){
        begin(9600);
        fSet(initialF);
        dSet(initialD);            
    },nullptr,H4P_TRID_SQWV);
}

void  H4P_ExternalSqWave::dSweep(uint32_t timer,uint32_t fromF,uint32_t toF,uint32_t inc,function<void(uint32_t)> onChange){
  _Sweep('d',H4ESW_MAX_D,timer,fromF,toF,inc,onChange);  
}

void  H4P_ExternalSqWave::fSweep(uint32_t timer,uint32_t fromF,uint32_t toF,uint32_t inc,function<void(uint32_t)> onChange){
  _Sweep('f',H4ESW_MAX_F,timer,fromF,toF,inc,onChange);  
}
