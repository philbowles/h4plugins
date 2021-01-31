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
    for(int i=0;i<32;i++){
        uint32_t inst=1 << i;
        if(t & inst) {
            string e=h4pgetEventName(static_cast<H4P_EVENT_TYPE>(inst));
            if(e.find("No ")==string::npos) h4pevt[inst].push_back(make_pair(pid,f));
        }
    }
}

void h4pemit(H4PID pid,H4P_EVENT_TYPE t,const char* msg){
    if(h4pevt.count(t)) for(auto const& e:h4pevt[t]) e.second(pid,t,msg);
    h4pGlobalEventHandler(pid,t,msg);
}

void h4pOnEvent(H4P_EVENT_TYPE t,H4P_FN_USEREVENT f){
    h4pregisterhandler(H4PID_SYS,static_cast<uint32_t>(t),[f](H4PID i,H4P_EVENT_TYPE t,const string& m){ f(m); });
}
//
//      Plugin names etc
//
H4Plugin* h4pptrfromtxt(const string& s){
    auto i=find(h4pnames.begin(),h4pnames.end(),s);
    return i!=h4pnames.end() ? h4pmap[distance(h4pnames.begin(),i)]:nullptr;
}

void h4pupcall(H4Plugin* me,H4Plugin* ptr){
    ptr->hookConnect([me](){ me->start(); });
    ptr->hookDisconnect([me](){ me->stop(); });
}
//
//
//      Verbosity
//
string h4pgetErrorMessage(uint32_t e){ auto vm=h4pisloaded<H4P_VerboseMessages>(H4PID_VM); return vm ? vm->getErrorMessage(e):string("Error "+stringFromInt(e)); }
string h4pgetEventName   (H4P_EVENT_TYPE e){ auto vm=h4pisloaded<H4P_VerboseMessages>(H4PID_VM); return vm ? vm->getEventName(e):string("E"+stringFromInt(e,"0x%08x")); }
string h4pgetTaskType    (uint32_t e){ auto vm=h4pisloaded<H4P_VerboseMessages>(H4PID_VM); return vm ? vm->getTaskType(e):string(stringFromInt(e,"%04d")); }
string h4pgetTaskName    (uint32_t e){ auto vm=h4pisloaded<H4P_VerboseMessages>(H4PID_VM); return vm ? vm->getTaskName(e):string(stringFromInt(e,"%04d")); }
//
//      Lifecycle callbacks
//
void h4FactoryReset(const string& src){
    h4psysevent(H4PID_SYS,H4P_EVENT_FACTORY,CSTR(stringFromInt(millis())));
    onFactoryReset();
    h4rebootCore();
}

void h4StartPlugins(){
    _cb[srcTag()]="SYS";
    _cb[h4pTag()]=H4P_VERSION;
    Serial.printf("H4P %s\n",CSTR(_cb[h4pTag()]));
//    for(auto const& p:h4pmap) Serial.printf("%s,",CSTR(p.second->_pName));
//    Serial.println();
    for(auto const& p:h4pmap) { p.second->_hookIn(); }
    for(auto const& p:h4pmap) p.second->_greenLight();
}
//
// General Purpose
//
string h4pGetConfig(const string& c){ return _cb[c]; }
int h4pGetConfigInt(const string& c){ return STOI(_cb[c]); }
void h4pSetConfig(const string& c,const string& v){ _cb[c]=v; }
void h4pSetConfig(const string& c,const int v){ _cb[c]=stringFromInt(v); }

string h4preplaceparams(const string& s){ // oh for a working regex!
	int i=0;
	int j=0;
	string rv;
    rv.reserve((s.size()*115)/100);
	while(i < s.size()){
        if(s[i]=='%'){
            if(j){
                string v=s.substr(j,i-j);
                if(_cb.count(v)) rv.append(_cb[v]);
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
/*
TESTERS / DIAG
*/
#if SANITY
void h4psanitycheck(){
    Serial.printf("SANITY CHECKS\n");
    std::map<uint32_t,string> names;
    int i=0;
    for(auto n:h4pnames){
        Serial.printf("name %02d %4s ptr=0x%08x\n",i,CSTR(n),h4pmap.count(i) ? (void*) h4pmap[i]:0 );
        names[i]=n;
        i++;
    }
    i=0;
    for(auto n:h4pmap) Serial.printf("i=%02d idx=%02d ptr=0x%08x name=%s\n",i++,n.first,(void*) n.second,CSTR(names[n.first]));

    Serial.printf("EVENTS LISTENED\n");
    unordered_set<uint32_t> listeners;
    unordered_map<uint32_t,vector<H4P_EVENT_TYPE>> earmap;
    for(auto et:h4pevt){
        Serial.printf("%s [0x%08x]\n",CSTR(h4pgetEventName(static_cast<H4P_EVENT_TYPE>(et.first))),et.first);
        for(auto el:et.second){
            uint32_t i=static_cast<int>(el.first);
            auto p=h4pmap.count(i) ? h4pmap[i]:nullptr;
            Serial.printf("\t%s(%d)\n",p ? CSTR(h4pmap[i]->_pName):"?",el.first);
            listeners.insert(i);
            earmap[i].push_back(static_cast<H4P_EVENT_TYPE>(et.first));
        }
    }
    Serial.printf("EVENTS LISTENERS\n");
    for(auto pid:listeners){
        auto p=h4pmap.count(pid) ? h4pmap[pid]:nullptr;
        Serial.printf("%s(%d)\n",p ? CSTR(h4pmap[pid]->_pName):"?",pid);
        for(auto e:earmap[pid]){
            Serial.printf("\t%s [0x%08x]\n",CSTR(h4pgetEventName(static_cast<H4P_EVENT_TYPE>(e))),e);
        }
    }
}
#endif