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
#include<H4P_VerboseMessages.h>

void __attribute__((weak)) onFactoryReset(){}
void __attribute__((weak)) onReboot(){}
void __attribute__((weak)) h4pGlobalEventHandler(H4PID pid,H4P_EVENT_TYPE t,const string& msg){}
//
//   Events Listeners Emitters
//
void h4pregisterhandler(H4PID pid,uint32_t t,H4P_FN_EVENTHANDLER f){
//    Serial.printf("h4pregisterhandler p=%d 0x%08x\n",pid,t);
    for(int i=0;i<32;i++){
        uint32_t inst=1 << i;
        if(t & inst) {
//            Serial.printf("%d REGISTER %08x\n",pid,inst);
            string e=h4pgetEventName(static_cast<H4P_EVENT_TYPE>(inst));
            if(e.find("No ")==string::npos) h4pevt[inst].push_back(make_pair(pid,f));
        }
    }
}
void h4pemit(H4PID pid,H4P_EVENT_TYPE t,const char* msg){
    if(h4pevt.count(t)) for(auto const& e:h4pevt[t]) e.second(pid,t,msg);
    h4pGlobalEventHandler(pid,t,msg);
}
//
//      Plugin names etc
//
H4Plugin* h4pptrfromtxt(const string& s){
    auto i=find(h4pnames.begin(),h4pnames.end(),uppercase(s));
    return i!=h4pnames.end() ? h4pmap[distance(h4pnames.begin(),i)]:nullptr;
}

void h4pupcall(H4Plugin* me,H4Plugin* ptr){
    ptr->hookConnect([me](){ me->start(); });
    ptr->hookDisconnect([me](){ me->stop(); });
}
void h4pdll(H4Plugin* dll){
    SEVENT(H4P_EVENT_DLL,"%s",CSTR(dll->_pName));
    dll->_hookIn();
}
//
//      Verbosity
//
string h4pgetErrorMessage(uint32_t e){ auto vm=h4pisloaded<H4P_VerboseMessages>(H4PID_VM); return vm ? vm->getErrorMessage(e):string("Error "+stringFromInt(e)); }
string h4pgetEventName   (H4P_EVENT_TYPE e){ auto vm=h4pisloaded<H4P_VerboseMessages>(H4PID_VM); return vm ? vm->getEventName(e):string("No "+stringFromInt(e)); }
string h4pgetTaskType    (uint32_t e){ auto vm=h4pisloaded<H4P_VerboseMessages>(H4PID_VM); return vm ? vm->getTaskType(e):string(stringFromInt(e,"%04d")); }
string h4pgetTaskName    (uint32_t e){ auto vm=h4pisloaded<H4P_VerboseMessages>(H4PID_VM); return vm ? vm->getTaskName(e):string(stringFromInt(e,"%04d")); }
//
//      Lifecycle cabacks
//
void h4FactoryReset(const string& src){
    h4psysevent(H4PID_SYS,H4P_EVENT_FACTORY,CSTR(stringFromInt(millis())));
    onFactoryReset();
    h4rebootCore();
}

void h4StartPlugins(){
//    Serial.printf("START THOSE %d PLUGINS!!!\n",h4pmap.size());
    H4Plugin::_cb[srcTag()]="SYS";
    H4Plugin::_cb[h4pTag()]=H4P_VERSION;
    Serial.printf("H4P %s\n",CSTR(H4Plugin::_cb[h4pTag()]));
    for(auto const& p:h4pmap) Serial.printf("INIT %d %s\n",p.first,CSTR(p.second->_pName));
//    for(auto const& p:h4pmap) p.second->_startup();
    for(auto const& p:h4pmap) p.second->_hookIn();
    for(auto const& p:h4pmap) p.second->_greenLight();
}
/*
TESTERS / DIAG
*/
void h4pevtdump(){
    Serial.printf("EVENTS LISTENED\n");
    unordered_set<uint32_t> listeners;
    unordered_map<uint32_t,vector<H4P_EVENT_TYPE>> earmap;
    for(auto et:h4pevt){
        Serial.printf("%s [0x%08x]\n",CSTR(h4pgetEventName(static_cast<H4P_EVENT_TYPE>(et.first))),et.first);
        for(auto el:et.second){
            uint32_t i=static_cast<int>(el.first);
            auto p=h4pmap.count(i) ? h4pmap[i]:nullptr;
            Serial.printf("\t%s(%d)\n",p ? CSTR(uppercase(h4pmap[i]->_pName)):"?",el.first);
            listeners.insert(i);
            earmap[i].push_back(static_cast<H4P_EVENT_TYPE>(et.first));
        }
    }
    Serial.printf("EVENTS LISTENERS\n");
    for(auto pid:listeners){
        auto p=h4pmap.count(pid) ? h4pmap[pid]:nullptr;
        Serial.printf("%s(%d)\n",p ? CSTR(uppercase(h4pmap[pid]->_pName)):"?",pid);
        for(auto e:earmap[pid]){
            Serial.printf("\t%s [0x%08x]\n",CSTR(h4pgetEventName(static_cast<H4P_EVENT_TYPE>(e))),e);
        }
    }
}