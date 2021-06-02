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
#include<H4Service.h>
#include<H4P_SerialCmd.h>

extern std::unordered_map<std::string,H4Service*> h4pmap;
std::string h4pSrc=h4pTag();

void __attribute__((weak)) h4pGlobalEventHandler(const std::string& svc,H4PE_TYPE t,const std::string& msg){}
//
//   Events Listeners Emitters
//
void h4pregisterhandler(const std::string& svc,uint32_t t,H4P_FN_EVENTHANDLER f){
    for(int i=0;i<32;i++){
        uint32_t inst=1 << i;
        if(t & inst) {
            if(h4pGetEventName(static_cast<H4PE_TYPE>(inst))!="") {
                auto dip=find_if(h4pevt[inst].begin(),h4pevt[inst].end(),[=](H4P_EVENT_LISTENER const& e){ return svc==e.first; });
                if(dip==h4pevt[inst].end()) h4pevt[inst].push_back(make_pair(svc,f)); // fix this
            }
        }
    }
}

void h4punregisterhandler(const std::string& svc,uint32_t t){
    h4pevt[t].erase(remove_if(h4pevt[t].begin(), h4pevt[t].end(), [=](H4P_EVENT_LISTENER el){ return el.first == svc; }), h4pevt[t].end());
    h4pevt[t].shrink_to_fit();
}

void h4pevent(const std::string& svc,H4PE_TYPE t,const std::string& msg){
    h4pGlobalEventHandler(svc,t,msg);
    if(h4pevt.count(t)) for(auto const& e:h4pevt[t]) e.second(svc,t,msg);
}

void h4pOnEvent(H4PE_TYPE t,H4P_FN_USEREVENT e){ h4pregisterhandler(userTag(),static_cast<uint32_t>(t),[e](const std::string& i,H4PE_TYPE t,const std::string& m){ e(m); }); }
//
//      Lifecycle callbacks
//
[[noreturn]] void h4pFactoryReset(){ h4psysevent(userTag(),H4PE_FACTORY,""); }// QEVENT(H4PE_FACTORY); }

[[noreturn]] void h4pReboot(){ h4psysevent(userTag(),H4PE_REBOOT,""); }

#ifdef ARDUINO_ARCH_ESP32
std::string getTerminalName(const std::string& s) {
//#if platform?
   char sep = '\\';
//#else
//   char sep = '/';
//#endif
   size_t i = s.rfind(sep, s.length());
   if (i != std::string::npos) return(s.substr(i+1, s.length() - i));
   return("");
}
#endif

#if SANITY
#include<H4P_PinMachine.h>
#include<H4P_GateKeeper.h>

void h4StartPlugins(){
    Serial.printf("Registered to start:\n");
    for(auto const& s:h4pmap) Serial.printf(" %s\n",CSTR(s.first));
    Serial.printf("GPIOs defined:\n");
    for(auto const& p:h4pPinMap) {
        Serial.printf(" %02d S=%d C=%d NPL=%d\n",p.first,p.second->_s,p.second->_c,p.second->_pipeline.size()); 
        p.second->dump();
    }
#else
void h4StartPlugins(){
#endif
    #ifdef ARDUINO_ARCH_ESP32
    std::string terminalname=getTerminalName(h4p[binTag()]); // sigh...just WHY????
    h4p[binTag()]=terminalname;
    #endif
    
    HAL_FS.begin();
    for(auto const& i:split(H4P_SerialCmd::read(glob()),RECORD_SEPARATOR)){
        std::vector<std::string> nv=split(i,UNIT_SEPARATOR);
        h4pGlobal[nv[0]]=h4proxy{nv[0],nv.size() > 1 ? nv[1]:"",true};
    }

    h4p.gvInc(NBootsTag());
    h4p.gvSave(NBootsTag());

    h4psysevent(h4pTag(),H4PE_SYSINFO,"%s",H4P_VERSION);
    h4pevent(h4pTag(),H4PE_BOOT);
    h4pevent(h4pTag(),H4PE_STAGE2);
//  must reverse reboot and factory chains so that cmd always gets last bite
    reverse(h4pevt[H4PE_REBOOT].begin(),h4pevt[H4PE_REBOOT].end());
    reverse(h4pevt[H4PE_FACTORY].begin(),h4pevt[H4PE_FACTORY].end());
//
#if SANITY
    for(auto const& s:h4pmap){
        auto ps=s.second;
        if(!ps->_running){
            if(ps->_parent!="") h4psysevent(h4pTag(),H4PE_SYSINFO,"%s waits %s\n",CSTR(s.first),CSTR(ps->_parent));
            else h4psysevent(h4pTag(),H4PE_SYSWARN,"%s ZOMBIE!!\n",CSTR(s.first));
        }
    }
#endif
//
//    reclaim some junk:
//      unfilters all services from boot/stage2 to prevent any chance of "double-dip"
//      unlinks event handlers for boot/stage2 since nothing can/will ever use them again (see above :) )
//
    for(auto e:std::initializer_list<H4PE_TYPE>{H4PE_BOOT,H4PE_STAGE2}){
        h4pevt.erase(e);
        h4pClearEvent(e);
        for(auto s:h4pmap) s.second->_filter&=~e;
    }
    for(auto &e:h4pevt) e.second.shrink_to_fit();

    h4psysevent(h4pTag(),H4PE_SYSINFO,"Ready: Heap=%u",_HAL_freeHeap());
}

std::string h4preplaceparams(const std::string& s){ // oh for a working regex!
	int i=0;
	int j=0;
	std::string rv;
    rv.reserve((s.size()*115)/100);
	while(i < s.size()){
        if(s[i]=='%'){
            if(j){
                std::string v=s.substr(j,i-j);
                if(h4p.gvExists(v)) rv.append(h4p[v]);
                else rv.append("%"+v+"%");
                j=0;
            }
            else j=i+1;
        } else if(!j) rv.push_back(s[i]);
        i++;
	}
    rv.shrink_to_fit();
	return rv.c_str();
}

void h4puiAdd(const std::string& n,H4P_UI_TYPE t,std::string h,const std::string& v,uint8_t c){ h4psysevent(h,H4PE_UIADD,"%s,%d,%s,%s,%d",CSTR(n),t,CSTR(v),CSTR(h),c); }

void h4puiSync(const std::string& n,const std::string& v){ h4psysevent(n,H4PE_UISYNC,"%s",CSTR(v)); }
/*
TESTERS / DIAG
*/
#if SANITY
void h4pInventory(){
    for(auto et:h4pevt){
        Serial.printf("%s [0x%08x] Listeners:\n",CSTR(h4pGetEventName(static_cast<H4PE_TYPE>(et.first))),et.first);
        for(auto el:et.second){
            std::string svc=el.first;
            Serial.printf("  %s\n",CSTR(svc));
        }
    }
    Serial.println();
    h4p.plugins();
}
#endif