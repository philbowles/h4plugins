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
#include<H4P_SerialCmd.h>
//
string                   H4P_SerialCmd::_fname="/";

extern std::unordered_map<string,H4Service*> h4pmap;
extern bool h4punlocked;

const char* __attribute__((weak)) giveTaskName(uint32_t id){ return "ANON"; }

H4P_SerialCmd::H4P_SerialCmd(bool autoStop): H4Service(cmdTag(),H4PE_FACTORY | H4PE_REBOOT){
    _fname.append(_me);
    _addLocals({
#if SANITY
        {"sanity",     { 0,         0, CMD(h4pInventory) }},
#endif
        {h4Tag(),      { 0,         H4PC_H4, nullptr}},
        {"clear",      { H4PC_H4,   0, CMD(clear)}},
        {"config",     { H4PC_H4,   0, CMDVS(_config)}},
        {"dump",       { H4PC_H4,   0, CMDVS(_dump)}},
        {"factory",    { H4PC_H4,   0, ([=](vector<string>){ h4psysevent(h4pSrc,H4PE_FACTORY,""); return H4_CMD_OK; }) }},
        {"get",        { H4PC_H4,   0, CMDVS(_get)}},
        {"reboot",     { H4PC_H4,   0, ([=](vector<string>){ h4psysevent(h4pSrc,H4PE_REBOOT,""); return H4_CMD_OK; }) }},
//
        {"svc",        { H4PC_H4,   H4PC_SVC, nullptr}},
        {"restart",    { H4PC_SVC,  0, CMDVS(_svcRestart) }},
        {"start",      { H4PC_SVC,  0, CMDVS(_svcStart) }},
        {stopTag(),    { H4PC_SVC,  0, CMDVS(_svcStop) }},
#if H4P_LOG_MESSAGES
        {"info",       { H4PC_SVC,  0, CMDVS(_svcInfo) }},
        {showTag(),    { H4PC_H4,   H4PC_SHOW, nullptr}},
        {allTag(),     { H4PC_SHOW, 0, CMD(all) }},
        {"fs",         { H4PC_SHOW, 0, CMD(showFS)}},
        {"globals",    { H4PC_SHOW, 0, CMD(info)}},
        {heapTag(),    { H4PC_SHOW, 0, CMD(heap) }},
        {"plugins",    { H4PC_SHOW, 0, CMD(plugins) }},
        {"q",          { H4PC_SHOW, 0, CMD(showQ) }},
#endif
        {"help",       { 0,         0, CMD(help) }}
    });
    if(autoStop) QTHIS(svcDown);
//    HAL_FS.begin();
}

H4P_CMDMAP_I H4P_SerialCmd::__exactMatch(const string& cmd,uint32_t owner){
    auto any=h4pCmdMap.equal_range(cmd);
    for(auto i=any.first;i!=any.second;i++) if(i->second.owner==owner) return i;
    return h4pCmdMap.end();
}

void H4P_SerialCmd::__flatten(function<void(string)> fn){
    H4P_CMDMAP_I ptr;
    for(ptr=h4pCmdMap.begin();ptr!=h4pCmdMap.end(); ptr++){
        if(!(ptr->second.owner)){
            if(ptr->second.levID) _flattenCmds(fn,ptr->first,ptr->first,ptr->second.levID);    
            else fn(ptr->first);
        } 
    }
}

uint32_t H4P_SerialCmd::_config(vector<string> vs){
    return _guard1(vs,[this](vector<string> vs){
        vector<string> ci=split(H4PAYLOAD,",");
        H4P_NVP_MAP pending;
        for(auto const& c:ci){
            vector<string> parts=split(c,"=");
            if(parts.size()>2) return H4_CMD_PAYLOAD_FORMAT;
            if(!h4p.gvExists(parts[0])) return H4_CMD_NAME_UNKNOWN;
            if(parts[0]==stateTag() && !h4punlocked) return H4_CMD_NOT_NOW; // dont like it but waaaaaaaay smaller / faster
            pending[parts[0]]=parts.size() >1 ? parts[1]:"";
        }
        for(auto const& p:pending) {
            h4p[p.first]=p.second;
            h4psysevent(_me,H4PE_UIMSG,"CONFIG: %s now=%s",CSTR(p.first),CSTR(p.second));
        }
        return H4_CMD_OK;
    });
}

uint32_t H4P_SerialCmd::_dispatch(vector<string> vs,uint32_t owner=0){
    if(vs.size()){
        H4P_CMDMAP_I i;
        string cmd=vs[0];
        i=__exactMatch(cmd,owner);
        if(i!=h4pCmdMap.end()){
            if(i->second.fn) return [=](){ return i->second.fn(CHOP_FRONT(vs)); }();
            else return _dispatch(CHOP_FRONT(vs),i->second.levID);
        } else return H4_CMD_UNKNOWN;
    } else return H4_CMD_UNKNOWN;
}

uint32_t H4P_SerialCmd::_executeCmd(string topic, string pload){
	vector<string> vs=split(CSTR(topic),"/");
    h4pSrc=vs[0];
	vs.push_back(pload);
    vector<string> cmd(vs.begin()+2,vs.end());
    XLOG("%s %s",CSTR(vs[0]),CSTR(join(cmd,"/")));
    uint32_t rv=_dispatch(vector<string>(cmd)); // optimise?
    if(rv) reply("%s",CSTR(h4pGetErrorMessage(rv)));
    return rv;
}

void H4P_SerialCmd::_flattenCmds(function<void(string)> fn,string cmd,string prefix,uint32_t lev){
    H4P_CMDMAP_I i=h4pCmdMap.find(cmd);
    for(i=h4pCmdMap.begin();i!=h4pCmdMap.end();i++){
        if(i->second.owner==lev){
			string trim = prefix+"/"+i->first;
			if(i->second.levID) _flattenCmds(fn,i->first,trim,i->second.levID);
			else fn(trim);
        }
    }
}

uint32_t H4P_SerialCmd::_get(vector<string> vs){
    return _guard1(vs,[this](vector<string> vs){
        if(h4p.gvExists(H4PAYLOAD)) {
            _showItem(H4PAYLOAD);
            return H4_CMD_OK;
        } else return H4_CMD_NAME_UNKNOWN;
    });
}

[[noreturn]] void H4P_SerialCmd::_handleEvent(const string& svc,H4PE_TYPE t,const string& msg){
    switch(t){
        case H4PE_FACTORY:
//            Serial.printf("MORITURI: DO NOT BELIEVE WHAT ARNIE SAYS!\n");
            clear(); 
        case H4PE_REBOOT:
//            Serial.printf("ARNIE SAYS: I'LL BE BACK!\n");
            h4rebootCore();
//        default:
//            Serial.printf("%s CATASTROPHE %s %s %s\n",CSTR(_me),CSTR(svc),CSTR(h4pGetEventName(t)),CSTR(msg));
    }
}

void H4P_SerialCmd::_init(){
    HAL_FS.begin();
    for(auto const& i:split(read(_fname),RECORD_SEPARATOR)){
        vector<string> nv=split(i,UNIT_SEPARATOR);
        h4pGlobal[nv[0]]=h4proxy{nv[0],nv.size() > 1 ? nv[1]:"",true};
//        Serial.printf("FROM PS: %s=%s\n",nv[0].data(),h4pGlobal[nv[0]].c_str());
    }
    gvInc(NBootsTag());
    gvSave(NBootsTag());
}

void H4P_SerialCmd::_persist(){
    string items;
    for(auto const& p:h4pGlobal) if(p.second._save) items+=p.second.get()+RECORD_SEPARATOR;
    if(items.size()){
        items.pop_back();
        if(h4p._running) write(_fname,items);
    } // else Serial.printf("**** NOWT TO SAVE!!!\n"); 
}

void H4P_SerialCmd::_run(){
    static string cmd="";
	static int	c;
    if((c=Serial.read()) != -1){
        if (c == '\n') {
            h4.queueFunction([=](){
                uint32_t rv=_simulatePayload(cmd);
                cmd="";
            },nullptr,H4P_TRID_SCMD);
        } else cmd+=c;
    }
}

uint32_t H4P_SerialCmd::_simulatePayload(string flat,const char* src){ // refac
    vector<string> vs=split(flat,"/");
    if(vs.size()){
		string pload=CSTR(H4PAYLOAD);
		vs.pop_back();
		string topic=join(vs,"/");
		return invokeCmd(topic,pload,src); // _invoke
	} else return H4_CMD_TOO_FEW_PARAMS;
}

uint32_t H4P_SerialCmd::_svcControl(H4P_SVC_CONTROL action,vector<string> vs){
    return _guard1(vs,[=](vector<string> vs){
        auto p=h4puncheckedcall<H4Service>(H4PAYLOAD);
        if(p) {
            switch(action){
#if H4P_LOG_MESSAGES
                case H4PSVC_STATE:
                    p->info();
                    break;
#endif
                case H4PSVC_STOP:
                    p->svcDown();
                    break;
                case H4PSVC_RESTART:
                    p->svcDown();
                case H4PSVC_START:
                    p->svcUp();
                    break;
            }
            return H4_CMD_OK;
        }
        return H4_CMD_NAME_UNKNOWN;
    });
}

uint32_t H4P_SerialCmd::_svcRestart(vector<string> vs){ return _svcControl(H4PSVC_RESTART,vs); }

uint32_t H4P_SerialCmd::_svcStart(vector<string> vs){ return _svcControl(H4PSVC_START,vs); }

uint32_t H4P_SerialCmd::_svcInfo(vector<string> vs){ return _svcControl(H4PSVC_STATE,vs); }

uint32_t H4P_SerialCmd::_svcStop(vector<string> vs){ return _svcControl(H4PSVC_STOP,vs); }
//
//
//
void H4P_SerialCmd::addCmd(const string& name,uint32_t owner, uint32_t levID,H4_FN_MSG f){
    if(__exactMatch(name,owner)==h4pCmdMap.end()) h4pCmdMap.insert(make_pair(name,command {owner,levID,f}));
}

void H4P_SerialCmd::clear(){
    h4pGlobal.clear();
    HAL_FS.remove(CSTR(_fname));
}

void H4P_SerialCmd::help(){ 
    vector<string> unsorted={};
    __flatten([&unsorted](string s){ unsorted.push_back(s); });
    sort(unsorted.begin(),unsorted.end());
    for(auto const& s:unsorted) { reply(CSTR(s)); }
}

uint32_t H4P_SerialCmd::invokeCmd(string topic,string payload,const char* src){ 
    return _executeCmd(string(src)+"/h4/"+CSTR(topic),string(CSTR(payload)));
}

uint32_t H4P_SerialCmd::invokeCmd(string topic,uint32_t payload,const char* src){ 
    return invokeCmd(topic,stringFromInt(payload),src);
}

string H4P_SerialCmd::read(const string& fn){
	string rv="";
        File f=HAL_FS.open(CSTR(fn), "r");
        if(f && f.size()) {
            int n=f.size();
            uint8_t* buff=(uint8_t *) malloc(n+1);
            f.readBytes((char*) buff,n);
            rv.assign((const char*) buff,n);
            free(buff);
        }
        f.close();
	return rv;	
}

void H4P_SerialCmd::removeCmd(const string& s,uint32_t pid){ if(__exactMatch(s,pid)!=h4pCmdMap.end()) h4pCmdMap.erase(s); }

uint32_t H4P_SerialCmd::write(const string& fn,const string& data,const char* mode){
    File b=HAL_FS.open(CSTR(fn), mode);
    b.print(CSTR(data));
    uint32_t rv=b.size(); // ESP32 pain
    b.close();
    return rv; 
}
// ifdef log events?

uint32_t H4P_SerialCmd::_dump(vector<string> vs){
    return _guard1(vs,[this](vector<string> vs){
        return ([this](string h){ 
            reply("DUMP FILE %s",CSTR(h));
            reply("%s",CSTR(read("/"+h)));
            return H4_CMD_OK;
        })(H4PAYLOAD);
    });
}

string H4P_SerialCmd::_dumpTask(task* t){
    char buf[128];
    uint32_t type=t->uid/100;
    uint32_t id=t->uid%100;
    sprintf(buf,"0x%08x %09lu %s/%s %s %9d %9d %9d",
        (void*) t,
        t->at,
        CSTR(h4pGetTaskType(type)),
        CSTR(h4pGetTaskName(id)),
        t->singleton ? "S":" ",
        t->rmin,
        t->rmax,
        t->nrq);
    return string(buf);
}

//
//      PERSISTENT STORAGE
//
void H4P_SerialCmd::_createProxy(const string& name,bool save){ 
    if(!h4pGlobal.count(name)){
//        Serial.printf("PROXY %s CREATED AS %s\n",CSTR(name),save ? "PERMANET":"TEMPORARY");
        h4pGlobal[name]=h4proxy(name,"",save);
    } 
}

void H4P_SerialCmd::_adjust(const string& name,int value){ 
    _createProxy(name);
    if(stringIsNumeric(h4p[name])){
        auto cv=atoi(CSTR(h4pGlobal[name]));
//        Serial.printf("ADJUST %s by %d cv=%d nv=%d\n",CSTR(name),value,cv,cv+value);
        h4pGlobal[name]=stringFromInt(cv+value);
    }
}

void H4P_SerialCmd::gvDec(const string& name){ _adjust(name,-1); }

void H4P_SerialCmd::gvErase(initializer_list<const char*> nil){ 
    bool    anypersistent=false;
    for(auto n:nil) {
        auto mts=h4pGlobal[n];
//        Serial.printf("PROXY %s DESTROYED AS %s\n",n,mts._save ? "PERMANET":"TEMPORARY");
        anypersistent|=mts._save;
        h4pGlobal.erase(n);
    }
    if(anypersistent) _persist();
}

string H4P_SerialCmd::gvGetstring(const string& name){ 
    if(h4pGlobal.count(name)) return h4pGlobal[name];
    return {};
}

int H4P_SerialCmd::gvGetInt(const string& name){ return atoi(CSTR(gvGetstring(name))); }

void H4P_SerialCmd::gvInc(const string& name){ _adjust(name,1); }

void H4P_SerialCmd::gvSave(initializer_list<const char*> save){ for(auto s:save) if(gvExists(s)) h4pGlobal[s]._save=true; }

void H4P_SerialCmd::gvSetstring(const string& name,const string& value,bool save){
    _createProxy(name,save);
    if(h4p[name]!=value) h4p[name]=value;
}

void H4P_SerialCmd::gvSetInt(const string& name,int value,bool save){ gvSetstring(name,stringFromInt(value),save); }
//
//
//
#if H4P_LOG_MESSAGES
void H4P_SerialCmd::all(){
    __flatten([this](string s){ 
        vector<string> candidates=split(s,"/");
        if(candidates.size()>1 && candidates[1]==showTag() && candidates[2]!=allTag()) {
            reply(CSTR(s));
            invokeCmd(s,"",CSTR(h4pSrc)); // snake / tail etc
            reply("");
        }
    });
}
#if H4P_LOG_MESSAGES
void H4P_SerialCmd::info(){ 
    H4Service::info();
    reply(" Globals:");
    for(auto const& p:h4pGlobal) _showItem(p.first);
}
#endif
void  H4P_SerialCmd::plugins(){ for(auto const& s:h4pmap) s.second->info(); }

#ifdef ARDUINO_ARCH_ESP8266
void H4P_SerialCmd::showFS(){
    uint32_t sigma=0;
    uint32_t n=0;

    FSInfo info;
    HAL_FS.info(info);
    reply(" totalBytes %d",info.totalBytes);
    reply(" usedBytes %d",info.usedBytes);
    reply(" blockSize %d",info.blockSize);
    reply(" pageSize %d",info.pageSize);
    reply(" maxOpenFiles %d",info.maxOpenFiles);
    reply(" maxPathLength %d",info.maxPathLength);
    
    Dir dir = HAL_FS.openDir("/");

    while (dir.next()) {
        n++;
        reply(" %s (%u)",CSTR(dir.fileName()),dir.fileSize());
        sigma+=dir.fileSize();
    }
    reply(" %d file(s) %u bytes",n,sigma);
}
#else
void H4P_SerialCmd::showFS(){
    uint32_t sigma=0;
    uint32_t n=0;

    reply(" totalBytes %d",HAL_FS.totalBytes());
    reply(" usedBytes %d",HAL_FS.usedBytes());

    File root = HAL_FS.open("/");
 
    File file = root.openNextFile();
 
    while(file){
        n++;
        reply( "%s (%u)",file.name(),file.size());
        sigma+=file.size();
        file = root.openNextFile();
    }
    reply(" %d file(s) %u bytes",n,sigma);
}
#endif // 8266/32 spiffs

void H4P_SerialCmd::showQ(){
	reply("           Due @tick Type              Min       Max       nRQ");  
    vector<task*> tlist=h4._copyQ();
    sort(tlist.begin(),tlist.end(),[](const task* a, const task* b){ return a->at < b->at; });
    for(auto const& t:tlist) reply(CSTR(_dumpTask(t)));
}
#endif
void H4P_SerialCmd::svcUp(){
    h4._hookLoop([this](){ _run(); },_pid); 
    H4Service::svcUp();
    _persist();
}

void H4P_SerialCmd::svcDown(){
    h4._unHook(_pid);
    H4Service::svcDown();
}
//
//      h4proxy
//
h4proxy& h4proxy::_set(const string& s){
//    Serial.printf("** %s ** string assign => %s (cv=%s)\n",CSTR(_id),CSTR(s),CSTR(_v));
    if(_v!=s){
        _v=s;
        if(_save) H4P_SerialCmd::_persist();
        h4psysevent(CSTR(_id),H4PE_GVCHANGE,"%s",CSTR(_v));
    }// else Serial.printf("** %s **NO CHANGE %s\n",CSTR(_id),CSTR(_v));
    return *this;
}

h4proxy& h4proxy::operator=(const h4proxy& s) {
    if(_id=="") {
        _id=s._id;
        _v=s._v;
        _save=s._save;
//        Serial.printf("VIRGIN %s created with value %s P/T=%d\n",CSTR(_id),CSTR(_v),_save);
        return *this; // creation: don't send change ?
    }
//    Serial.printf("EXISTING %s updated to %s\n",CSTR(_id),CSTR(s._v));
    return _set(s._v);
}