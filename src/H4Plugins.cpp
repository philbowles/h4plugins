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
#include<H4PCommon.h>
#include<H4P_SerialCmd.h>

void __attribute__((weak)) onFactoryReset(){}
//
vector<string> h4pnames={
    "VM",
    "CMD",
    "1SEC",
    "ESQW",
    "EVTQ",
    "HEAP",
    "EAR0",
    "HWRN",
    "LLOG",
    "LOOP",
    "ONOF",
    "QWRN",
    "SLOG",
    "SNIF",
    "STOR",
    "TONE",
    "GPIO",
    "WINK",
    "WIFI",
    "BEAT",
    "MQTT",
    "MLG0",
    "UPNP",
    "MFNB",
    "PDI0",
    "PDM0",
    "PDU0",
    "RUPD",
    "SQLL",
    "SSET",
    "TIME",
    "UDPL",
    "XXX0"
};

void H4Plugin::_applyEventFilter(const string &m,H4P_EVENT_TYPE t,const string& s){ 
    if(t & _eventFilter) _handleEvent(m,t,s);
}
/*
uint32_t H4Plugin::frig(){
    auto i=find(h4pnames.begin(),h4pnames.end(),uppercase(_pName));
    if(i!=h4pnames.end()){
        uint32_t pos=distance(h4pnames.begin(),i);
        if(!h4pmap[pos]) {
            h4pmap[pos]=this;
            return pos;
        }
        else Serial.printf("DOUBLE DIPPERS! %s\n",CSTR(_pName));
    } else Serial.printf("WASSSSSSSSSAP!!!!!!!! %s\n",CSTR(_pName));
    return 999;
}
*/
H4Plugin* H4Plugin::isLoaded(const string& x){
    for(auto const& p:h4pmap) if(p.second->_pName==x) return p.second;
    return nullptr;
}

void H4Plugin::_dynamicLoad(H4Plugin* p){
//    Serial.printf("_dynamicLoad %d %08x\n",p->_subCmd-H4PC_MAX,(void*) p);
    p->_startup();
    p->_hookIn();
    h4cmd.emitEvent(H4P_EVENT_DLL,_pName,"%s",CSTR(p->_pName));
}

void h4StartPlugins(){
    H4Plugin::_cb[srcTag()]=userTag();
    H4Plugin::_cb[h4pTag()]=H4P_VERSION;
    Serial.printf("H4P %s\n",CSTR(H4Plugin::_cb[h4pTag()]));
//    Serial.printf("CALIBRATION? cal=%u nL=%u peak=%u\n",H4_CALIBRATE,h4Nloops,h4Nloops * (1000 / H4_CALIBRATE));
//    for(auto const& p:h4pmap) { Serial.printf("map sez %08x %s in pos %d\n",(void*) p.second,(CSTR(h4pnames[p.first])),p.first); }
    for(auto const& p:h4pmap) p.second->_startup();
//    for(auto const& p:h4pmap) { Serial.printf("post startup sez %08x %s in pos %d\n",(void*) p.second,(CSTR(h4pnames[p.first])),p.first); }
    for(auto const& p:h4pmap) p.second->_hookIn();
//    for(auto const& p:h4pmap) { Serial.printf("post hookin sez %08x %s in pos %d\n",(void*) p.second,(CSTR(h4pnames[p.first])),p.first); }
    for(auto const& p:h4pmap) p.second->_greenLight();
//    for(auto const& p:h4pmap) { Serial.printf("post greenlight sez %08x %s in pos %d\n",(void*) p.second,(CSTR(h4pnames[p.first])),p.first); }
}

void h4FactoryReset(const string& src){
    SYSEVENT(H4P_EVENT_FACTORY,src,stringFromInt(millis()));
    onFactoryReset();
    h4rebootCore();
}

vector<uint32_t> H4Plugin::expectInt(string pl,const char* delim){
    vector<uint32_t> results;
    vector<string> tmp=split(pl,delim);
    for(auto const& t:tmp){
        if(!stringIsNumeric(t)) return {};
        results.push_back(STOI(t));
    }
    return results;
}

uint32_t H4Plugin::guardInt1(vector<string> vs,function<void(uint32_t)> f){
    return guard1(vs,[f,this](vector<string> vs){
        auto vi=expectInt(H4PAYLOAD);
        if(vi.size()==1) return ([f](uint32_t v){ f(v); return H4_CMD_OK; })(vi[0]);
        return H4_CMD_NOT_NUMERIC;
    });
}        

uint32_t H4Plugin::guardString2(vector<string> vs,function<H4_CMD_ERROR(string,string)> f){
    return guard1(vs,[f,this](vector<string> vs){
        auto vg=split(H4PAYLOAD,",");
        if(vg.size()<3){ 
            if(vg.size()>1) return ([f](string s1,string s2){ return f(s1,s2); })(vg[0],vg[1]);
            return H4_CMD_TOO_FEW_PARAMS;
        }
        return H4_CMD_TOO_MANY_PARAMS;
    });
}

void H4Plugin::_startup(){
    _commands.insert(_cmds.begin(),_cmds.end());
    _cmds.clear();
    h4cmd._hookLogChain(bind(&H4Plugin::_applyEventFilter,this,_1,_2,_3));
}

void H4Plugin::start() {
    if(!state()){
        _up=true;
        _start(); // call the overrideable
    }
}

void H4Plugin::stop() {
    if(state()){
        _up=false;
        _stop(); // call the overrideable
    }
}

void H4Plugin::_upHooks(){ 
    SYSEVENT(H4P_EVENT_SVC_UP,_pName,"");
    for(auto const& c:_connected) c();
}

void H4Plugin::_downHooks(){
    for(auto const& c:_disconnected) c();
    SYSEVENT(H4P_EVENT_SVC_DOWN,_pName,"");
}

//void H4PEventListener::_hookIn(){ h4cmd._hookLogChain(bind(&H4PEventListener::_filterEvent,this,_1,_2,_3)); }