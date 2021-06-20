/*
 MIT License

Copyright (c) 2020 Phil Bowles <H4Services@gmail.com>
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
//
#if H4_HOOK_TASKS

    H4_INT_MAP h4TaskNames={
        {H4P_TRID_THRB,"THRB"},
        {H4P_TRID_PATN,"PATN"},
        {H4P_TRID_PP1x,"PP1X"},
        {H4P_TRID_PWM1,"PWM1"},
        {H4P_TRID_DBNC,"DBNC"},
        {H4P_TRID_RPTP,"RPTP"},
        {H4P_TRID_POLL,"POLL"},
        {H4P_TRID_HOTA,"HOTA"},
        {H4P_TRID_MQMS,"MQMS"},
        {H4P_TRID_MQRC,"MQRC"},
        {H4P_TRID_REST,"REST"},
        {H4P_TRID_SOAP,"SOAP"},
        {H4P_TRID_UDPS,"UDPS"},
        {H4P_TRID_NTFY,"UNFY"},
        {H4P_TRID_SCMD,cmdTag()},
        {H4P_TRID_HLOG,"HLOG"},
        {H4P_TRID_QLOG,"QLOG"},
        {H4P_TRID_BTTO,"BTTO"},
        {H4P_TRID_IPPD,"IPPD"},
        {H4P_TRID_DALY,"DALY"},
        {H4P_TRID_SHOT,"AT_T"},
        {H4P_TRID_SSET,"SSET"},
        {H4P_TRID_SYNC,"SYNC"},
        {H4P_TRID_TIME,uppercase(timeTag())},
        {H4P_TRID_GATE,"GATE"},
        {H4P_TRID_BOOT,"BOOT"},
        {H4P_TRID_D0,"D000"},
        {H4P_TRID_D1,"D001"},
        {H4P_TRID_D2,"D002"},
        {H4P_TRID_D3,"D003"},
        {H4P_TRID_UPKT,"UPKT"},
        {H4P_TRID_D5,"D005"},
        {H4P_TRID_D6,"D006"},
        {H4P_TRID_D7,"D007"},
        {H4P_TRID_D8,"D008"},
        {H4P_TRID_D9,"D009"}
    };
#endif

H4P_EVENT_HANDLERS h4pevt;

void H4Service::_addLocals(H4P_CMDMAP local){
    h4pCmdMap.insert(local.begin(),local.end());
    local.clear();
}

void H4Service::_envoi(const std::string& s){
    auto pp=h4puncheckedcall<H4Service>(h4pSrc);
    if(pp) pp->_reply(CSTR(s)); // send reply back to originating source
    else Serial.printf("%s\n",CSTR(s));
}

std::vector<uint32_t> H4Service::_expectInt(std::string pl,const char* delim){
    std::vector<uint32_t> results;
    std::vector<std::string> tmp=split(pl,delim);
    for(auto const& t:tmp){
        if(!stringIsNumeric(t)) return {};
        results.push_back(STOI(t));
    }
    return results;
}

uint32_t H4Service::_guard1(std::vector<std::string> vs,H4_FN_MSG f){
    if(!vs.size()) return H4_CMD_TOO_FEW_PARAMS;
    return vs.size()>1 ? H4_CMD_TOO_MANY_PARAMS:f(vs);
}

uint32_t H4Service::_guardInt1(std::vector<std::string> vs,std::function<void(uint32_t)> f){
    return _guard1(vs,[f,this](std::vector<std::string> vs){
        auto vi=_expectInt(H4PAYLOAD);
        if(vi.size()==1) return ([f](uint32_t v){ f(v); return H4_CMD_OK; })(vi[0]);
        return H4_CMD_NOT_NUMERIC;
    });
}

uint32_t H4Service::_guardString2(std::vector<std::string> vs,std::function<H4_CMD_ERROR(std::string,std::string)> f){
    return _guard1(vs,[f,this](std::vector<std::string> vs){
        auto vg=split(H4PAYLOAD,",");
        if(vg.size()<3){ 
            if(vg.size()>1) return ([f](std::string s1,std::string s2){ return f(s1,s2); })(vg[0],vg[1]);
            return H4_CMD_TOO_FEW_PARAMS;
        }
        return H4_CMD_TOO_MANY_PARAMS;
    });
}

void H4Service::_sysHandleEvent(const std::string& svc,H4PE_TYPE t,const std::string& msg){
    switch(t){
        case H4PE_BOOT:
            if(_filter & H4PE_BOOT) _init();
            break;
        case H4PE_STAGE2:
            if(!(_filter & H4PE_SERVICE)) svcUp(); // not waiting to be started
            break;
        case H4PE_SERVICE:
            if(_parent==svc){
                if(STOI(msg)) svcUp();
                else svcDown();
            } else _handleEvent(svc,H4PE_SYSINFO,std::string("Svc").append(STOI(msg) ? "Up":"Down"));
            break;
        case H4PE_GVCHANGE:
            if(h4pevt.count(H4PE_BOOT)) return;
        default:
            _handleEvent(svc,t,msg);
            break;
    }
}

std::string H4Service::_uniquify(const std::string& name,uint32_t uqf){
    std::string tmp=name+(uqf ? stringFromInt(uqf):"");
    return h4pmap.count(tmp) ? _uniquify(name,uqf+1):tmp;
}
#if H4P_LOG_MESSAGES
    #if H4P_VERBOSE
        extern H4_INT_MAP eventTypes;
    #endif
void H4Service::info(){ 
    reply("\nSVC: %s PID=%d %sRunning",CSTR(_me),_pid,_running ? "":"Not ");
    if(_parent!="") reply(" Depends on %s",CSTR(_parent));
    //
    std::vector<std::string> dees;
    for(auto const& s:h4pmap) if(s.second->_parent==_me) dees.push_back(s.first);
    if(dees.size()){
        reply(" Dependees");
        for(auto const& d:dees) reply("  %s",CSTR(d));
    }
    //
    #if H4P_VERBOSE
    if(_filter){
        reply(" Listens for");
        for(int i=0;i<32;i++){
            auto shift=1 << i;
            if( eventTypes.count(shift) && (_filter & shift)) reply("  %s",CSTR(h4pGetEventName(static_cast<H4PE_TYPE>(shift))));
        }
    }
    #endif // verbose
}
#endif // log messages