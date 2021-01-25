/*
 MIT License

Copyright (c) 2020 Phil Bowles <h4plugins@gmail.com>
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
#include<H4P_BinaryThing.h>
#include<H4P_WiFi.h>
#include<H4P_AsyncMQTT.h>

void H4P_BinaryThing::_greenLight() {
    _pWiFi=h4pisloaded<H4P_WiFi>(H4PID_WIFI);
    _pMQTT=h4pisloaded<H4P_AsyncMQTT>(H4PID_MQTT);
    if(_pWiFi){
        if(WiFi.getMode()==WIFI_STA) {
            _pWiFi->_uiAdd(H4P_UIO_ONOF,onofTag(),H4P_UI_ONOF,"",[this]{ return stringFromInt(state()); });
            if(_pMQTT) _pMQTT->hookConnect([this](){ _pMQTT->subscribeDevice("slave/#",CMDVS(_slave),H4PC_H4); }); // addcmd slave
        }
    }
}

void H4P_BinaryThing::_publish(bool b){ if(_pMQTT) _pMQTT->publishDevice(stateTag(),b); }

void H4P_BinaryThing::_setSlaves(bool b){
    for(auto s:_slaves){
        string t=s+"/h4/switch";
        if(_pMQTT) _pMQTT->xPublish(CSTR(t),stringFromInt(b));
    }
}

void H4P_BinaryThing::_setState(bool b) {
    _state=b;
    _cb[stateTag()]=stringFromInt(b);
    _setSlaves(b);
    if(_pWiFi) _pWiFi->uiSync();
}

uint32_t H4P_BinaryThing::_slave(vector<string> vs){
    if(vs.size()<2) return H4_CMD_TOO_FEW_PARAMS;
    if(vs.size()>2) return H4_CMD_TOO_MANY_PARAMS;
    if(!stringIsAlpha(vs[0])) return H4_CMD_PAYLOAD_FORMAT;
    if(!stringIsNumeric(H4PAYLOAD)) return H4_CMD_NOT_NUMERIC;
    if(H4PAYLOAD_INT > 1) return H4_CMD_OUT_OF_BOUNDS;
    slave(vs[0],H4PAYLOAD_INT);
    PLOG("%s slave %s",H4PAYLOAD_INT ? "Added":"Removed",CSTR(vs[0]));
    show();
    return H4_CMD_OK;
}

void H4P_BinaryThing::_start() { 
    H4Plugin::_start();
    if(_f) _f(_state);
    turn(_state);
}

#if H4P_LOG_EVENTS
void H4P_BinaryThing::_turn(bool b,const string& src){
    h4.cancelSingleton(H4P_TRID_BTTO);
    if(b && _timeout) h4.once(_timeout,[this](){ _turn(OFF,"btto"); },nullptr,H4P_TRID_BTTO,true);
    if(b!=_state){
        _setState(b);
        if(_f) _f(_state);
        _publish(_state);
//        PLOG("%s %s",CSTR(src),(_state ? "ON":"OFF")); // specialise
        PEVENT(_state ? H4P_EVENT_ON:H4P_EVENT_OFF,"%s",CSTR(src)); // specialise
    }
}

void H4P_BinaryThing::turn(bool b){ _turn(b,userTag()); }
// syscall only
#else
void H4P_BinaryThing::turn(bool b){
    h4.cancelSingleton(H4P_TRID_BTTO);
    if(b && _timeout) h4.once(_timeout,[this](){ turn(OFF); },nullptr,H4P_TRID_BTTO,true);
    if(b!=_state){
        _setState(b);
        if(_f) _f(_state);
        _publish(_state);
    }
}
//
//      H4P_ConditionalThing
//
void H4P_ConditionalThing::_hookIn() {
    _pWiFi->_uiAdd(H4P_UIO_COND,conditionTag(),H4P_UI_BOOL,"",[this]{ return stringFromInt(_predicate(state())); });
    H4P_BinaryThing::_hookIn();
}

void H4P_ConditionalThing:: _setState(bool b) { 
    if(_predicate(b)) H4P_BinaryThing::_setState(b);
    else _pWiFi->uiMessage("Unable: condition disarmed");
}

void H4P_ConditionalThing::syncCondition() { _pWiFi->_sendSSE(conditionTag(),CSTR(stringFromInt(_predicate(state())))); }

#endif
