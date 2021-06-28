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
// just for the versions
#include<vark_config.h>
#include<dillo_config.h>
#include<pango_config.h>
//
extern std::unordered_map<std::string,H4Service*> h4pmap;
extern bool h4punlocked;

const char* __attribute__((weak)) giveTaskName(uint32_t id){ return "ANON"; }

H4P_SerialCmd::H4P_SerialCmd(bool autoStop): H4Service(cmdTag(),H4PE_FACTORY | H4PE_REBOOT){
    h4p.gvSetstring(chipTag(),_HAL_uniqueName(""));
    h4p.gvSetstring(boardTag(),replaceAll(H4_BOARD,"ESP8266_",""));
    h4p.gvSetstring(h4pTag(),H4P_VERSION);

    _addLocals({
#if SANITY
        {"sanity",     { 0,         0, CMD(h4pInventory) }},
#endif
        {h4Tag(),      { 0,         H4PC_H4, nullptr}},
        {"clear",      { H4PC_H4,   0, CMD(clear)}},
        {"config",     { H4PC_H4,   0, CMDVS(_config)}},
        {"dump",       { H4PC_H4,   0, CMDVS(_dump)}},
        {"factory",    { H4PC_H4,   0, ([=](std::vector<std::string>){ h4psysevent(h4pSrc,H4PE_FACTORY,""); return H4_CMD_OK; }) }},
        {"get",        { H4PC_H4,   0, CMDVS(_get)}},
        {"reboot",     { H4PC_H4,   0, ([=](std::vector<std::string>){ h4psysevent(h4pSrc,H4PE_REBOOT,""); return H4_CMD_OK; }) }},
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
    #if H4_HOOK_TASKS
        {"q",          { H4PC_SHOW, 0, CMD(showQ) }},
    #endif
#endif
        {"help",       { 0,         0, CMD(help) }}
    });
    if(autoStop) h4._unHook(_pid);
    std::string ino=h4p[binTag()];
    ino+="."+lowercase(H4_BOARD)+".bin";
    h4p[binTag()]=ino;
}

H4P_CMDMAP_I H4P_SerialCmd::__exactMatch(const std::string& cmd,uint32_t owner){
    auto any=h4pCmdMap.equal_range(cmd);
    for(auto i=any.first;i!=any.second;i++) if(i->second.owner==owner) return i;
    return h4pCmdMap.end();
}

void H4P_SerialCmd::__flatten(std::function<void(std::string)> fn){
    H4P_CMDMAP_I ptr;
    for(ptr=h4pCmdMap.begin();ptr!=h4pCmdMap.end(); ptr++){
        if(!(ptr->second.owner)){
            if(ptr->second.levID) _flattenCmds(fn,ptr->first,ptr->first,ptr->second.levID);    
            else fn(ptr->first);
        } 
    }
}

uint32_t H4P_SerialCmd::_config(std::vector<std::string> vs){
    return _guard1(vs,[this](std::vector<std::string> vs){
        std::vector<std::string> ci=split(H4PAYLOAD,",");
        H4P_NVP_MAP pending;
        for(auto const& c:ci){
            std::vector<std::string> parts=split(c,"=");
            if(parts.size()>2) return H4_CMD_PAYLOAD_FORMAT;
            if(!h4p.gvExists(parts[0])) SYSWARN("CONFIG %s=%s name not known\n",parts[0].data(),parts[1].data());
            else {
                if(parts[0]==stateTag() && !h4punlocked) return H4_CMD_NOT_NOW; // dont like it but waaaaaaaay smaller / faster
                pending[parts[0]]=parts.size() >1 ? parts[1]:"";
            }
        }
        for(auto const& p:pending) h4p[p.first]=p.second;
        return H4_CMD_OK;
    });
}

uint32_t H4P_SerialCmd::_dispatch(std::vector<std::string> vs,uint32_t owner=0){
    if(vs.size()){
        H4P_CMDMAP_I i;
        std::string cmd=vs[0];
        i=__exactMatch(cmd,owner);
        if(i!=h4pCmdMap.end()){
            if(i->second.fn) return [=](){ return i->second.fn(CHOP_FRONT(vs)); }();
            else return _dispatch(CHOP_FRONT(vs),i->second.levID);
        } else return H4_CMD_UNKNOWN;
    } else return H4_CMD_UNKNOWN;
}

uint32_t H4P_SerialCmd::_executeCmd(std::string topic, std::string pload){
	std::vector<std::string> vs=split(CSTR(topic),"/");
    h4pSrc=vs[0];
	vs.push_back(pload);
    std::vector<std::string> cmd(vs.begin()+2,vs.end());
    XLOG("%s %s",CSTR(vs[0]),CSTR(join(cmd,"/")));
    uint32_t rv=_dispatch(std::vector<std::string>(cmd)); // optimise?
    if(rv) reply("%s",CSTR(h4pGetErrorMessage(rv)));
    return rv;
}

void H4P_SerialCmd::_flattenCmds(std::function<void(std::string)> fn,std::string cmd,std::string prefix,uint32_t lev){
    H4P_CMDMAP_I i=h4pCmdMap.find(cmd);
    for(i=h4pCmdMap.begin();i!=h4pCmdMap.end();i++){
        if(i->second.owner==lev){
			std::string trim = prefix+"/"+i->first;
			if(i->second.levID) _flattenCmds(fn,i->first,trim,i->second.levID);
			else fn(trim);
        }
    }
}

uint32_t H4P_SerialCmd::_get(std::vector<std::string> vs){
    return _guard1(vs,[this](std::vector<std::string> vs){
        if(h4p.gvExists(H4PAYLOAD)) {
            _showItem(H4PAYLOAD);
            return H4_CMD_OK;
        } else return H4_CMD_NAME_UNKNOWN;
    });
}

void H4P_SerialCmd::_handleEvent(const std::string& svc,H4PE_TYPE t,const std::string& msg){
    switch(t){
        case H4PE_FACTORY:
            clear(); 
        case H4PE_REBOOT:
            h4rebootCore();
    }
}

void H4P_SerialCmd::_persist(){
    std::string items;
    for(auto const& p:h4pGlobal) if(p.second._save) items+=p.second.get()+RECORD_SEPARATOR;
    if(items.size()){
        items.pop_back();
        if(h4p._running) write(glob(),items);
    }
}

void H4P_SerialCmd::_run(){
    static std::string cmd="";
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

uint32_t H4P_SerialCmd::_simulatePayload(std::string flat,const char* src){ // refac
    // a bit of hoop-jumping to allow / characters in simulated payloads
    std::vector<std::string> bt=split(flat,"\`");
    std::string f2;
    if(bt.size() > 1){
		std::string esc=replaceAll(bt.back(),"/","\`");
        bt.pop_back();
        f2=join(bt,"/")+esc;
    } else f2=flat;
    std::vector<std::string> vs=split(f2,"/");
    if(vs.size()){
		std::string pload=replaceAll(vs.back(),"\`","/");
		vs.pop_back();
		std::string topic=join(vs,"/");
		return invokeCmd(topic,pload,src); // _invoke
	} else return H4_CMD_TOO_FEW_PARAMS;
}

uint32_t H4P_SerialCmd::_svcControl(H4P_SVC_CONTROL action,std::vector<std::string> vs){
    return _guard1(vs,[=](std::vector<std::string> vs){
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

uint32_t H4P_SerialCmd::_svcRestart(std::vector<std::string> vs){ return _svcControl(H4PSVC_RESTART,vs); }

uint32_t H4P_SerialCmd::_svcStart(std::vector<std::string> vs){ return _svcControl(H4PSVC_START,vs); }

uint32_t H4P_SerialCmd::_svcInfo(std::vector<std::string> vs){ return _svcControl(H4PSVC_STATE,vs); }

uint32_t H4P_SerialCmd::_svcStop(std::vector<std::string> vs){ return _svcControl(H4PSVC_STOP,vs); }
//
//
//
void H4P_SerialCmd::addCmd(const std::string& name,uint32_t owner, uint32_t levID,H4_FN_MSG f){
    if(__exactMatch(name,owner)==h4pCmdMap.end()) h4pCmdMap.insert(make_pair(name,command {owner,levID,f}));
}

void H4P_SerialCmd::clear(){
    h4pGlobal.clear();
    HAL_FS.remove(glob());
}

void H4P_SerialCmd::help(){ 
    std::vector<std::string> unsorted={};
    __flatten([&unsorted](std::string s){ unsorted.push_back(s); });
    sort(unsorted.begin(),unsorted.end());
    for(auto const& s:unsorted) { reply(CSTR(s)); }
}

uint32_t H4P_SerialCmd::invokeCmd(std::string topic,std::string payload,const char* src){ return _executeCmd(std::string(src)+"/h4/"+CSTR(topic),std::string(CSTR(payload))); }

uint32_t H4P_SerialCmd::invokeCmd(std::string topic,uint32_t payload,const char* src){ return invokeCmd(topic,stringFromInt(payload),src); }

std::string H4P_SerialCmd::read(const std::string& fn){
	std::string rv="";
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

void H4P_SerialCmd::removeCmd(const std::string& s,uint32_t pid){ if(__exactMatch(s,pid)!=h4pCmdMap.end()) h4pCmdMap.erase(s); }

uint32_t H4P_SerialCmd::write(const std::string& fn,const std::string& data,const char* mode){
    File b=HAL_FS.open(CSTR(fn), mode);
    b.print(data.data());
    b.close();
    return data.size(); // fix this!!!!!!!!!!!!
}

uint32_t H4P_SerialCmd::_dump(std::vector<std::string> vs){
    return _guard1(vs,[this](std::vector<std::string> vs){
        return ([this](std::string h){ 
            reply("DUMP FILE %s",CSTR(h));
            reply("%s",CSTR(read("/"+h)));
            return H4_CMD_OK;
        })(H4PAYLOAD);
    });
}

void H4P_SerialCmd::_createProxy(const std::string& name,bool save){ if(!h4pGlobal.count(name)) h4pGlobal[name]=h4proxy(name,"",save); }

void H4P_SerialCmd::_adjust(const std::string& name,int value){ 
    _createProxy(name);
    if(stringIsNumeric(h4p[name])){
        auto cv=atoi(CSTR(h4pGlobal[name]));
        h4pGlobal[name]=stringFromInt(cv+value);
    }
}

void H4P_SerialCmd::gvDec(const std::string& name){ _adjust(name,-1); }

void H4P_SerialCmd::gvErase(std::initializer_list<const char*> nil){ 
    bool    anypersistent=false;
    for(auto n:nil) {
        auto mts=h4pGlobal[n];
        anypersistent|=mts._save;
        h4pGlobal.erase(n);
    }
    if(anypersistent) _persist();
}

std::string H4P_SerialCmd::gvGetstring(const std::string& name){ 
    if(h4pGlobal.count(name)) return h4pGlobal[name];
    return {};
}

int H4P_SerialCmd::gvGetInt(const std::string& name){ return atoi(CSTR(gvGetstring(name))); }

void H4P_SerialCmd::gvInc(const std::string& name){ _adjust(name,1); }

void H4P_SerialCmd::gvSave(std::initializer_list<const char*> save){ for(auto s:save) if(gvExists(s)) h4pGlobal[s]._save=true; }

void H4P_SerialCmd::gvSetstring(const std::string& name,const std::string& value,bool save){
    _createProxy(name,save);
    if(h4p[name]!=value) h4p[name]=value;
}

void H4P_SerialCmd::gvSetInt(const std::string& name,int value,bool save){ gvSetstring(name,stringFromInt(value),save); }
//
#if H4P_LOG_MESSAGES
void H4P_SerialCmd::all(){
    __flatten([this](std::string s){ 
        std::vector<std::string> candidates=split(s,"/");
        if(candidates.size()>1 && candidates[1]==showTag() && candidates[2]!=allTag()) {
            reply(CSTR(s));
            invokeCmd(s,"",CSTR(h4pSrc)); // snake / tail etc
            reply("");
        }
    });
}

void H4P_SerialCmd::info(){ 
    H4Service::info();
    reply(" H4 %s",H4_VERSION);
    reply(" H4P %s",h4p[h4pTag()].data());
    reply(" H4UI %s",h4p.read("/h4UI").data());
    reply(" AARD %s",AARDVARK_VERSION);
    reply(" ARMA %s",ARMADILLO_VERSION);
    reply(" PANGO %s",PANGO_VERSION);
    reply(" Globals:");
    for(auto const& p:h4pGlobal) _showItem(p.first);
}

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

#endif // logmessages

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
h4proxy& h4proxy::_set(const std::string& s){
    if(_v!=s){
        _v=s;
        if(_save) H4P_SerialCmd::_persist();
        h4psysevent(CSTR(_id),H4PE_GVCHANGE,"%s",CSTR(_v));
    }
    return *this;
}

h4proxy& h4proxy::operator=(const h4proxy& s) {
    if(_id=="") {
        _id=s._id;
        _v=s._v;
        _save=s._save;
        return *this; // creation: don't send change ?
    }
    return _set(s._v);
}