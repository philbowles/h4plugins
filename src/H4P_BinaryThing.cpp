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
#include<H4P_AsyncWebServer.h>
#include<H4P_AsyncMQTT.h>

void H4P_BinaryThing::_hookIn() {
    if(isLoaded(aswsTag())) h4asws._uiAdd(onofTag(),H4P_UI_ONOF,[this]{ return stringFromInt(state()); });
    if(isLoaded(mqttTag())) {
        _cb[stateTag()]=stringFromInt(state());
//        h4mqtt.hookConnect([this](){ _publish(_state); }); 
        h4mqtt.subscribeDevice("slave/#",CMDVS(_slave),H4PC_H4);
        h4mqtt.addReportingItem(stateTag());
        h4mqtt.addReportingItem(autoTag());
    }
}

void H4P_BinaryThing::_publish(bool b){ if(isLoaded(mqttTag())) h4mqtt.publishDevice(stateTag(),b); }

void H4P_BinaryThing::_setSlaves(bool b){
    for(auto s:_slaves){
        string t=s+"/h4/switch";
        h4mqtt.xPublish(CSTR(t),stringFromInt(b));
    }
}

void H4P_BinaryThing::_setState(bool b) {
    _state=b;
    _cb[stateTag()]=stringFromInt(b);
    _setSlaves(b);
    if(isLoaded(aswsTag())) h4asws.uiSync(); //h4asws.uiSync(onofTag());}
}

uint32_t H4P_BinaryThing::_slave(vector<string> vs){
    if(vs.size()<2) return H4_CMD_TOO_FEW_PARAMS;
    if(vs.size()>2) return H4_CMD_TOO_MANY_PARAMS;
    if(!stringIsAlpha(vs[0])) return H4_CMD_PAYLOAD_FORMAT;
    if(!isNumeric(H4PAYLOAD)) return H4_CMD_NOT_NUMERIC;
    if(H4PAYLOAD_INT > 1) return H4_CMD_OUT_OF_BOUNDS;
    if(H4PAYLOAD_INT) _slaves.insert(vs[0]);
    else _slaves.erase(vs[0]);
    H4EVENT("%s slave %s",H4PAYLOAD_INT ? "Added":"Removed",CSTR(vs[0]));
    show();
    return H4_CMD_OK;
}
void H4P_BinaryThing::_start() { 
    H4Plugin::_start();
    if(_f) _f(_state);
    turn(_state);
}

#ifdef H4P_LOG_EVENTS
void H4P_BinaryThing::_turn(bool b,const string& src){
    h4.cancelSingleton(H4P_TRID_BTTO);
    if(b && _timeout) h4.once(_timeout,[this](){ _turn(OFF,"btto"); },nullptr,H4P_TRID_BTTO,true);
    if(b!=_state){
        _setState(b);
        if(_f) _f(_state);
        _publish(_state);
        SYSEVENT(H4P_LOG_H4,src,onofTag(),"%s",(_state ? "ON":"OFF"));
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
    if(isLoaded(aswsTag())) h4asws._uiAdd(ConditionTag(),H4P_UI_BOOL,[this]{ return stringFromInt(_predicate(state())); });
    H4P_BinaryThing::_hookIn();
}

void H4P_ConditionalThing:: _setState(bool b) { 
    if(_predicate(b)) H4P_BinaryThing::_setState(b);
    else if(isLoaded(aswsTag())) h4asws.uiMessage("Unable: condition disarmed");
}

void H4P_ConditionalThing::syncCondition() { if(isLoaded(aswsTag())) h4asws._sendSSE(ConditionTag(),CSTR(stringFromInt(_predicate(state())))); }

#endif
