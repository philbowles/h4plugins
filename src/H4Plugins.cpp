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
#include<H4PCommon.h>
#include<H4P_SerialCmd.h>

void __attribute__((weak)) h4AddAwsHandlers(){}
void __attribute__((weak)) onFactoryReset(){}

vector<H4_FN_VOID>  H4Plugin::_factoryChain;
//
//
//
bool stringIsAlpha(const string& s){ return !(std::find_if(s.begin(), s.end(),[](char c) { return !std::isalpha(c); }) != s.end()); }
//
void h4StartPlugins(){
//    Serial.printf("STARTING %d plugins\n",H4Plugin::_plugins.size());
    for(auto const& p:H4Plugin::_plugins) { p->_startup(); }
    for(auto const& p:H4Plugin::_plugins) { p->_hookIn(); }
    for(auto const& p:H4Plugin::_plugins) { p->_greenLight(); }
    reverse(H4Plugin::_factoryChain.begin(),H4Plugin::_factoryChain.end());
    H4Plugin::_hookFactory(onFactoryReset);
}

void h4FactoryReset(){
    for(auto &c:H4Plugin::_factoryChain) c();
    h4reboot();
}

vector<uint32_t> H4Plugin::expectInt(string pl,const char* delim){
    vector<uint32_t> results;
    vector<string> tmp=split(pl,delim);
    for(auto const& t:tmp){
        if(!isNumeric(t)) return {};
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

uint32_t H4Plugin::guardString2(vector<string> vs,function<void(string,string)> f){
    return guard1(vs,[f,this](vector<string> vs){
        auto vg=split(H4PAYLOAD,",");
        if(vg.size()<3){ 
            if(vg.size()>1) return ([f](string s1,string s2){ f(s1,s2); return H4_CMD_OK; })(vg[0],vg[1]);
            return H4_CMD_TOO_FEW_PARAMS;
        }
        return H4_CMD_TOO_MANY_PARAMS;
    });
}

void H4Plugin::reply(const char* fmt,...){ // find pub sub size
    char buff[H4P_REPLY_BUFFER+1];
    va_list ap; 
    va_start(ap, fmt); 
    vsnprintf(buff,H4P_REPLY_BUFFER,fmt,ap);
    va_end(ap);
    string source=_cb[srcTag()];
    H4Plugin* p=isLoaded(source);
    if(p) p->_reply(buff);
    else Serial.println(buff);
}

void H4Plugin::_startup(){
//    Serial.printf("H4Plugin::_startup %s state=%d\n",CSTR(_pName),state()); 
    _commands.insert(_cmds.begin(),_cmds.end());
    _cmds.clear();
}

void H4Plugin::start() {
//    Serial.printf("H4Plugin::start %s state=%d\n",CSTR(_pName),state()); 
    if(!state()){
        H4EVENT("svc start %s",CSTR(_pName));
        _start(); // call the overrideable
    } //else Serial.printf("%s already started\n",CSTR(_pName));
}

void H4Plugin::stop() { 
    if(state()){
        _stop(); // call the overrideable
        H4EVENT("svc stop %s",CSTR(_pName));
    } //else Serial.printf("%s already stopped\n",CSTR(_pName));
}

void H4Plugin::_upHooks(){ 
    _up=true;
    SYSEVENT(H4P_LOG_SVC_UP,"svc",_pName,"");
    for(auto const& c:_connected) c();
}

void H4Plugin::_downHooks(){ 
    for(auto const& c:_disconnected) c();
    SYSEVENT(H4P_LOG_SVC_DOWN,"svc",_pName,"");
    _up=false;
}
//
//      H4PlogService
//
void H4PLogService::_hookIn(){
    //REQUIRE(scmd);
    h4cmd._hookLogChain(bind(&H4PLogService::_filterLog,this,_1,_2,_3,_4));
}
