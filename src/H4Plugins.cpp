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
#include<H4P_MQTT.h>
#include<H4P_AsyncWebServer.h>

void __attribute__((weak)) h4AddAwsHandlers(){}
void __attribute__((weak)) onFactoryReset(){}

vector<H4_FN_VOID>  H4PluginService::_factoryChain;

void h4StartPlugins(){
    for(auto const& p:H4Plugin::_plugins) p->_startup();
    for(auto const& p:H4Plugin::_plugins) { p->_hookIn(); }
    for(auto const& p:H4Plugin::_plugins) p->_greenLight();
    H4PluginService::hookFactory(onFactoryReset);
}

void h4FactoryReset(){
    for(auto &c:H4PluginService::_factoryChain) c();
    h4reboot();
}

H4Plugin::H4Plugin(){
    subid=++nextSubid;
    _plugins.push_back(this);
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
        auto vi=expectInt(PAYLOAD);
        if(vi.size()==1) return ([f](uint32_t v){ f(v); return H4_CMD_OK; })(vi[0]);
        else return H4_CMD_NOT_NUMERIC;
    });
}        

uint32_t H4Plugin::guardInt4(vector<string> vs,function<void(uint32_t,uint32_t,uint32_t,uint32_t)> f){
    return guard1(vs,[f,this](vector<string> vs){
        auto vi=expectInt(PAYLOAD);
        if(vi.size()==4) return ([f](uint32_t v1,uint32_t v2,uint32_t v3,uint32_t v4){ f(v1,v2,v3,v4); return H4_CMD_OK; })(vi[0],vi[1],vi[2],vi[3]);
        else return H4_CMD_NOT_NUMERIC;
    });
}

uint32_t H4Plugin::guardString2(vector<string> vs,function<void(string,string)> f){
    return guard1(vs,[f,this](vector<string> vs){
        auto vg=split(PAYLOAD,",");
        if(vg.size()<3){ 
            if(vg.size()>1) return ([f](string s1,string s2){ f(s1,s2); return H4_CMD_OK; })(vg[0],vg[1]);
            return H4_CMD_TOO_FEW_PARAMS;
        }
        else return H4_CMD_TOO_MANY_PARAMS;
    });
} 

void H4Plugin::reply(const char* fmt,...){ // find pub sub size
    char buff[128];
    va_list ap; 
    va_start(ap, fmt); 
    vsnprintf(buff,127,fmt,ap);
    va_end(ap);
    string source=_cb[srcTag()];
    #ifndef H4P_NO_WIFI
        if(source==aswsTag()) h4asws._reply(buff);
        else { 
            if(source==mqttTag()) h4mqtt._reply(buff); 
            else {
    #endif
                H4Plugin::_reply(buff);
    #ifndef H4P_NO_WIFI
            }
        }
    #endif
}

void H4Plugin::_startup(){
    h4._hookLoop(_hook,_names,subid);
    if(_cmds.size()) commands.insert(_cmds.begin(),_cmds.end());
    _cmds.clear();
    _names.clear();
}
//
//      H4PluginService
//
void H4PluginService::_startup(){
    _cmds={
            {"restart", { 0, 0, CMD(restart)}},
            {"start",   { 0, 0, CMD(start)}},
            {"stop",    { 0, 0, CMD(stop)}}
        };
    for(auto &c:_cmds) c.second.owner=subid;
    h4sc._addCmd(_pid,{H4PC_ROOT,subid,nullptr});
    _cmds.insert(_local.begin(),_local.end());
    _local.clear();
    H4Plugin::_startup();
}

void H4PluginService::h4pcConnected(){ 
    for(auto const& c:_connChain) c();
    svc(_pid,H4P_LOG_SVC_UP);
}

void H4PluginService::h4pcDisconnected(){
    for(auto const& d:_discoChain) d();
    svc(_pid,H4P_LOG_SVC_DOWN);
}

void H4PluginService::svc(const string& uid,H4P_LOG_TYPE ud) {
    #ifdef H4P_LOG_EVENTS 
        if(H4Plugin::isLoaded(scmdTag())) {
            h4sc._logEvent(uid,ud,"svc","h4");
            Serial.print("SVC ");Serial.print(CSTR(uid));
            Serial.print(" ");Serial.println(ud==H4P_LOG_SVC_UP ? "UP":"DOWN");
        }
    #endif
}
//
//      H4PlogService
//
void H4PLogService::_hookIn(){ 
    h4sc._hookLogChain(bind(&H4PLogService::_filterLog,this,_1,_2,_3,_4));
    h4sc.addCmd(msgTag(),subid, 0, CMDNULL);
}
