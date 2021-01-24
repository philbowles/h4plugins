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
//
vector<string> h4pnames={
    "SYS", // ROOT
    "R1", // H4
    "R2", // SHOW
    "SVC", // SVC
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
    "GPIO", //
    "WINK",
    "WIFI", //
    "BEAT",
    "MQTT", //
    "MLG0",
    "UPNP", //
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
void H4Plugin::_envoi(const string& s){
    string source=_cb[srcTag()];
    auto pp=h4pptrfromtxt(source);
    if(pp) pp->_reply(CSTR(s)); // send reply back to originating source
    else Serial.printf("SRC %s %s\n",CSTR(source),CSTR(s));
}

vector<uint32_t> H4Plugin::_expectInt(string pl,const char* delim){
    vector<uint32_t> results;
    vector<string> tmp=split(pl,delim);
    for(auto const& t:tmp){
        if(!stringIsNumeric(t)) return {};
        results.push_back(STOI(t));
    }
    return results;
}

uint32_t H4Plugin::_guardInt1(vector<string> vs,function<void(uint32_t)> f){
    return _guard1(vs,[f,this](vector<string> vs){
        auto vi=_expectInt(H4PAYLOAD);
        if(vi.size()==1) return ([f](uint32_t v){ f(v); return H4_CMD_OK; })(vi[0]);
        return H4_CMD_NOT_NUMERIC;
    });
}        

uint32_t H4Plugin::_guardString2(vector<string> vs,function<H4_CMD_ERROR(string,string)> f){
    return _guard1(vs,[f,this](vector<string> vs){
        auto vg=split(H4PAYLOAD,",");
        if(vg.size()<3){ 
            if(vg.size()>1) return ([f](string s1,string s2){ return f(s1,s2); })(vg[0],vg[1]);
            return H4_CMD_TOO_FEW_PARAMS;
        }
        return H4_CMD_TOO_MANY_PARAMS;
    });
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
    SEVENT(H4P_EVENT_SVC_UP,_pName,"");
    for(auto const& c:_connected) c();
}

void H4Plugin::_downHooks(){
    for(auto const& c:_disconnected) c();
    SEVENT(H4P_EVENT_SVC_DOWN,_pName,"");
}