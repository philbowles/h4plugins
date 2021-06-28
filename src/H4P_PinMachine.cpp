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
#pragma once

#include<H4P_PinMachine.h>

void H4P_PinMachine::_run(){ 
    for(auto const& p:h4pPinMap) {
        auto ptr=p.second;
        uint32_t metal=ptr->isAnalogInput() ? analogRead(p.first):digitalRead(p.first);
        if(metal!=ptr->_r) ptr->inject(metal,false);
    }
}

void H4P_PinMachine::all(bool b){ for(auto const& p:h4pPinMap) if(isOutput(p.first)) static_cast<h4pOutput*>(p.second)->turn(b); }

#if H4P_LOG_MESSAGES
void H4P_PinMachine::info(){
    H4Service::info();
    for(auto const& p:h4pPinMap) reply("%s",CSTR(p.second->dump()));
}
#endif

void H4P_PinMachine::svcUp(){
    for(auto const& p:h4pPinMap) {
        Serial.printf("START PINS: %s\n",p.second->dump().data());
        p.second->_announce();
    }
    h4._hookLoop([this](){ _run(); },_pid);
    H4Service::svcUp();
}

void H4P_PinMachine::svcDown(){
    h4._unHook(_pid);
    H4Service::svcDown();
}

uint8_t     H4P_PinMachine::logicalRead  (uint8_t p)                     { return delegate<uint8_t> (p,&h4pGPIO::logicalRead); }
int         H4P_PinMachine::getValue     (uint8_t p)                     { return delegate<uint32_t>(p,&h4pGPIO::getValue); }
msg         H4P_PinMachine::inject       (uint8_t p,uint32_t r,bool t)   { return delegate<msg>     (p,&h4pGPIO::inject,r,t); }
bool        H4P_PinMachine::isAnalogInput(uint8_t p)                     { return delegate<bool>    (p,&h4pGPIO::isAnalogInput); }
bool        H4P_PinMachine::isOutput     (uint8_t p)                     { return delegate<bool>    (p,&h4pGPIO::isOutput); }
void        H4P_PinMachine::logicalWrite (uint8_t p,bool b){ 
    auto ptr = isManaged(p); // do the others too
    if(ptr && ptr->isOutput()) static_cast<h4pOutput*>(ptr)->logicalWrite(b);
}
H4_TIMER    H4P_PinMachine::periodicRead (uint8_t p,uint32_t f)          {
    return h4.every(f,[=]{ H4P_PinMachine::inject(p,H4P_PinMachine::isAnalogInput(p) ? analogRead(p):digitalRead(p),true); },nullptr,H4P_TRID_POLL);
}
msg         H4P_PinMachine::runFlow      (uint8_t p,msg m)               { return delegate<msg>     (p,&h4pGPIO::runFlow,m); }
//==============================================================================================================================================
//
//      PIN STUFF
//
h4pGPIO::h4pGPIO(uint8_t p,uint8_t m,H4PM_SENSE s,npFLOW flow): _p(p),_s(s),_pipeline(flow){
    _pipeline.shrink_to_fit();
    require<H4P_PinMachine>(gpioTag());
    if(!h4pPinMap.count(p)){
        pinMode(p,m);
        h4pPinMap[p]=this;
        _syncValue(); // _r = physical, _v=logical
    }
}

void h4pGPIO::_announce(){
    _prev=(*npPublishValue)(_prev);
    h4pregisterhandler(stringFromInt(_p,"%02d"),H4PE_VIEWERS,[=](const std::string& s,H4PE_TYPE t,const std::string& m){ _handleEvent(s,t,m); });
}

void h4pGPIO::_handleEvent(const std::string& s,H4PE_TYPE t,const std::string& msg){
    switch(t){ // refactor? signaller?
        case H4PE_VIEWERS:
            if(STOI(msg)) h4puiAdd(stringFromInt(_p,"%02d"),H4P_UI_GPIO,"g","",_c);
    }
}

#if H4P_LOG_MESSAGES
std::string h4pGPIO::dump(){
    char* buff=static_cast<char*>(malloc(H4P_REPLY_BUFFER+1));
    snprintf(buff,H4P_REPLY_BUFFER," P%02d S=%s P=%d L=%d C=%s V=%d O=%d A=%d",_p,_s ? "HI":"LO",digitalRead(_p),logicalRead(),h4pGetLedColor(_c).data(),getValue(),isOutput(),isAnalogInput());
    std::string rv(buff);
    free(buff);
    return rv;
}
#endif

msg h4pGPIO::inject(uint32_t metal,bool timer){ // P is so we can fake it e.f. for encoders
    _r=metal; // so the runner can recognise physical change... maybe node-ise it?
    unsigned long   nowuS=micros();
    return _prev=runFlow({_p,nowuS,nowuS-_prev.time,_normalise(metal),metal,false,timer,_prev.sigE});
}

msg h4pGPIO::runFlow(msg seed){
    msg tmp=seed;
    for(auto node:_pipeline) {
        tmp=(*node)(tmp); // <-- 
        if(tmp.halt) break;
    }
    return tmp;
}