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
#include<H4Plugins.h>

extern void h4FactoryReset();

H4_CMD_MAP_I H4P_SerialCmd::__exactMatch(const string& cmd,uint32_t owner){
    auto any=commands.equal_range(cmd);
    for(auto i=any.first;i!=any.second;i++) if(i->second.owner==owner) return i;
    return commands.end();
}

void H4P_SerialCmd::removeCmd(const string& s,uint32_t subid){ if(__exactMatch(s,subid)!=commands.end()) commands.erase(s); }

void H4P_SerialCmd::__flatten(function<void(string)> fn){
    H4_CMD_MAP_I ptr;
    for(ptr=commands.begin();ptr!=commands.end(); ptr++){
        if(!(ptr->second.owner)){
            if(ptr->second.levID) _flattenCmds(fn,ptr->first,ptr->first,ptr->second.levID);    
            else fn(ptr->first);
        } 
    }
}

uint32_t H4P_SerialCmd::_dispatch(vector<string> vs,uint32_t owner=0){
    if(vs.size()){
        H4_CMD_MAP_I i;
        string cmd=vs[0];
        i=__exactMatch(cmd,owner);
        if(i!=commands.end()){
            if(i->second.fn) return (bind(i->second.fn,CHOP_FRONT(vs)))();
            else return _dispatch(CHOP_FRONT(vs),i->second.levID);
        } else return H4_CMD_UNKNOWN;
    } else return H4_CMD_UNKNOWN;
}

void H4P_SerialCmd::_flattenCmds(function<void(string)> fn,string cmd,string prefix,uint32_t lev){
    H4_CMD_MAP_I i=commands.find(cmd);
    for(i=commands.begin();i!=commands.end();i++){
        if(i->second.owner==lev){
			string trim = prefix+"/"+i->first;
			if(i->second.levID) _flattenCmds(fn,i->first,trim,i->second.levID);
			else fn(trim);
        }
    }
}

uint32_t H4P_SerialCmd::_simulatePayload(string flat,const char* src){ // refac
	vector<string> vs=split(flat,"/");
    if(vs.size()){
		string pload=CSTR(PAYLOAD);
		vs.pop_back();
		string topic=join(vs,"/");
		return invokeCmd(topic,pload,src);			
	}
    else return H4_CMD_TOO_FEW_PARAMS; // really?
}
//

uint32_t H4P_SerialCmd::_unload(vector<string> vs){ 
    return guard1(vs,[this](vector<string> vs){
        return ([this](string s){
            H4Plugin* p=H4Plugin::isLoaded(s);
            if(p) {
                unload(p->subid);
                showUnload();
                return H4_CMD_OK;
            }
            return H4_CMD_NAME_UNKNOWN;
        })(PAYLOAD);
    });
}
//
//      public
//
void H4P_SerialCmd::all(){
    __flatten([this](string s){ 
        vector<string> candidates=split(s,"/");
        if(candidates.size()>1 && candidates[1]=="show" && candidates[2]!="all") invokeCmd(s,"",CSTR(_cb[srcTag()])); // snake / tail etc
    });
}

void H4P_SerialCmd::_logEvent(const string &msg,H4P_LOG_TYPE type,const string& source,const string& target){
//    Serial.printf("H4P_SerialCmd::_logEvent %s -> %d loggers\n",CSTR(msg),_logChain.size());
    for(auto const& l:_logChain) l(msg,type,source,target); // if 0 serial print
}

uint32_t H4P_SerialCmd::_executeCmd(string topic, string pload){
//    Serial.printf("_executeCmd %s\n",CSTR(topic));
	vector<string> vs=split(CSTR(topic),"/");
    _cb[srcTag()]=vs[0];
    _cb["target"]=vs[1];
	vs.push_back(pload);
    vector<string> cmd(vs.begin()+2,vs.end());
    #ifdef H4P_LOG_EVENTS
//        Serial.printf("_executeCmd 2 %s\n",CSTR(topic));
        _logEvent(join(cmd,"/"),H4P_LOG_CMD,vs[0],vs[1]);
    #endif	
    uint32_t rv=_dispatch(vector<string>(cmd)); // optimise?
    #ifdef H4P_LOG_EVENTS
        if(rv) _logEvent(_errorString(rv),H4P_LOG_ERROR,vs[0],vs[1]);
    #endif
    return rv;
}

void H4P_SerialCmd::help(){ 
    vector<string> unsorted={};
    __flatten([&unsorted](string s){ unsorted.push_back(s); });
    sort(unsorted.begin(),unsorted.end());
    for(auto const& s:unsorted) { reply("%s\n",CSTR(s)); }
}

uint32_t H4P_SerialCmd::invokeCmd(string topic,string payload,const char* src){ 
    return _executeCmd(string(src)+"/h4/"+CSTR(topic),string(CSTR(payload)));
}

uint32_t H4P_SerialCmd::invokeCmd(string topic,uint32_t payload,const char* src){ 
    return invokeCmd(topic,stringFromInt(payload),src);
}

void H4P_SerialCmd::unload(const uint32_t subid){
    if(H4::unloadables.count(subid)) {
        h4._unHook(H4::unloadables[subid]);
        H4::unloadables.erase(subid);
    }
}	
//
//      H4P_SerialCmd
//
void H4P_SerialCmd::_hookIn(){
#ifndef ARDUINO_ARCH_STM32    
//    if(!SPIFFS.begin()) Serial.println("Warning: NO SPIFFS");
    SPIFFS.begin();
#endif
}

H4P_SerialCmd::H4P_SerialCmd(){
    _pid=scmdTag();
    _hook=[this](){ run(); };
    _names={ {H4P_TRID_SCMD,uppercase(_pid)} };
    _cmds={
        {"h4",         { 0, H4PC_ROOT, nullptr}},
        {"help",       { 0, 0, CMD(help) }},
        {"reboot",     { H4PC_ROOT, 0, CMD(h4reboot) }},
        {"factory",    { H4PC_ROOT, 0, CMD(h4FactoryReset) }},
        {"show",       { H4PC_ROOT, H4PC_SHOW, nullptr}},
        {"all",        { H4PC_SHOW, 0, CMD(all) }},
        {"config",     { H4PC_SHOW, 0, CMD(config) }},
        {"plugins",    { H4PC_SHOW, 0, CMD(plugins) }},
#ifndef ARDUINO_ARCH_STM32
        {"spif",       { H4PC_SHOW, 0, CMD(showSPIFFS)}},
        {"dump",       { H4PC_ROOT, 0, CMDVS(_dump)}},
#endif
//      diag below here        
        {"q",          { H4PC_SHOW, 0, CMD(dumpQ) }},
        {"qstats",     { H4PC_SHOW, 0, CMD(Qstats) }},
        {"tnames",     { H4PC_SHOW, 0, CMD(tnames) }},
        {"unload",     { H4PC_SHOW, 0, CMD(showUnload) }},
        {"unload",     { H4PC_ROOT, 0, CMDVS(_unload) }}
    };
};
// diag
void H4P_SerialCmd::dumpQ(){ h4.dumpQ(); }

void H4P_SerialCmd::logEventType(H4P_LOG_TYPE t,const string& fmt,...){
    char buff[256];
    va_list ap; 
    va_start(ap, fmt); 
    vsnprintf(buff,255,CSTR(fmt),ap);
    va_end(ap);
    _logEvent(buff,t,userTag(),"h4");
}

void H4P_SerialCmd::plugins(){ for(auto const& p:H4Plugin::_plugins) reply("P: %s ID=%d\n",CSTR(p->_pid),p->subid); }

void H4P_SerialCmd::Qstats(){ reply("Q capacity: %u size: %u\n",h4._capacity(),h4.size()); }

void H4P_SerialCmd::showUnload(){ for(auto const& u:H4::unloadables) reply("U: %d=%s\n",u.first,CSTR(H4Plugin::pidFromSubid(u.first))); }

void H4P_SerialCmd::tnames(){ for(auto const& n:H4::trustedNames) { reply("T: %d=%s\n",n.first,CSTR(n.second)); }}
//
string H4P_SerialCmd::_errorString(uint32_t err){
    return H4Plugin::isLoaded(cerrTag()) ? h4ce.getErrorMessage(err):"Error: "+stringFromInt(err);
}
void H4P_SerialCmd::run(){ // halting loop optimise
    static string cmd="";
	static int	c;

    if((c=Serial.read()) != -1){
        if (c == '\n') {
            h4.queueFunction(bind([this](string cmd){
                uint32_t err=_simulatePayload(cmd);
                if(err) reply("%s\n",CSTR(_errorString(err)));
            },cmd),nullptr,H4P_TRID_SCMD);
            cmd="";
        } else cmd+=c;		
    }	
}
#ifndef ARDUINO_ARCH_STM32

string H4P_SerialCmd::read(const string& fn){
	string rv="";
        File f=SPIFFS.open(CSTR(fn), "r");
        if(f && f.size()) {
            int n=f.size();
            uint8_t* buff=(uint8_t *) malloc(n+1);
            f.readBytes((char*) buff,n);
            rv=stringFromBuff(buff,n);
            free(buff);
        }
        f.close();
	return rv;	
}

uint32_t H4P_SerialCmd::write(const string& fn,const string& data,const char* mode){
    File b=SPIFFS.open(CSTR(fn), mode);
    b.print(CSTR(data));
    uint32_t rv=b.size(); // ESP32 pain
    b.close();
    return rv; 
}

uint32_t H4P_SerialCmd::_dump(vector<string> vs){
    return guard1(vs,[this](vector<string> vs){
        return ([this](string h){ 
            reply("DUMP FILE %s\n",CSTR(h));
            reply("%s\n",CSTR(read("/"+h)));
            return H4_CMD_OK;
        })(PAYLOAD);
    });
}

#ifdef ARDUINO_ARCH_ESP8266

void H4P_SerialCmd::showSPIFFS(){
    uint32_t sigma=0;
    uint32_t n=0;

    FSInfo info;
    SPIFFS.info(info);
    reply("totalBytes %d\n",info.totalBytes);    
    reply("usedBytes %d\n",info.usedBytes);    
    
    Dir dir = SPIFFS.openDir("/");

    while (dir.next()) {
        n++;
        reply("FILE: %s %u\n",CSTR(dir.fileName()),dir.fileSize());
        sigma+=dir.fileSize();
    }
    reply("\n %d file(s) %u bytes\n",n,sigma);
}
#else
void H4P_SerialCmd::showSPIFFS(){
    uint32_t sigma=0;
    uint32_t n=0;

    File root = SPIFFS.open("/");
 
    File file = root.openNextFile();
 
    while(file){
        n++;
        reply("FILE: %s %u\n",file.name(),file.size());
        sigma+=file.size();
        file = root.openNextFile();
    }
    reply("\n %d file(s) %u bytes\n",n,sigma);
}
#endif
#endif
