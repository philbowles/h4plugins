/*
 MIT License

Copyright (c) 2020 Phil Bowles <H48266@gmail.com>
   github     https://github.com/philbowles/H4
   blog       https://8266iot.blogspot.com
   groups     https://www.facebook.com/groups/esp8266questions/
              https://www.facebook.com/H4-Esp8266-Firmware-Support-2338535503093896/


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
#pragma once

#include<H4.h>
#include<pmbtools.h>
#include<config_plugins.h>
#include<h4proxy.h>

#ifdef ARDUINO_ARCH_ESP8266
    #include<LittleFS.h>
    #define HAL_FS LittleFS
#else
    #include<FS.h>
    #include<SPIFFS.h>
    #define HAL_FS SPIFFS
    std::string getTerminalName(const std::string& s);
#endif

//void  _HAL_analogWrite(uint8_t pin, uint32_t value);
//void  _HAL_analogFrequency(uint8_t pin,size_t f);

#include<unordered_set>
#include<map>
//
#define STAG(x) constexpr const char* x##Tag(){ return #x; }

STAG(age);
STAG(all);
STAG(autoOff);
STAG(bin);
STAG(board)
STAG(broker);
STAG(change);
STAG(chip);
STAG(cmd);
STAG(device);
STAG(gpio);
STAG(h4);
STAG(heap);
STAG(ip);
STAG(mqconf);
STAG(mQpass);
STAG(mqtt);
STAG(mQuser);
STAG(name);
STAG(NBoots);
STAG(onof);
STAG(pcent);
STAG(psk); // chg password
STAG(report);
STAG(show);
STAG(snif);
STAG(ssid);
STAG(state);
STAG(stop);
STAG(tcp);
STAG(tick);
STAG(time);
STAG(upnp);
STAG(upTime);
STAG(user);
STAG(wifi);
STAG(wink);

constexpr const char* cmdhash(){ return "/h4/#"; }

enum H4PE_TYPE:uint32_t {
    H4PE_NOOP,
    H4PE_SYSINFO   = 1 <<  0,
    H4PE_SYSWARN   = 1 <<  1,
    H4PE_SYSFATAL  = 1 <<  2,
    H4PE_BOOT      = 1 <<  3,
    H4PE_SERVICE   = 1 <<  4,
    H4PE_MSG       = 1 <<  5,
    H4PE_REBOOT    = 1 <<  6,
    H4PE_UIADD     = 1 <<  7,
    H4PE_FACTORY   = 1 <<  8,
    H4PE_CMD       = 1 <<  9,
    H4PE_HEAP      = 1 << 10,
    H4PE_Q         = 1 << 11,
    H4PE_LOOPS     = 1 << 12,
    H4PE_PRESENCE  = 1 << 13,
    H4PE_GVCHANGE  = 1 << 14,
    H4PE_UISYNC    = 1 << 15,
    H4PE_STAGE2    = 1 << 16,
    H4PE_GPIO      = 1 << 17,
    H4PE_SIGNAL    = 1 << 18,
    H4PE_VIEWERS   = 1 << 19,
    H4PE_RTC       = 1 << 20,
    H4PE_ALARM     = 1 << 21,
    H4PE_GRID      = 1 << 22,
    H4PE_UPNP      = 1 << 23,
    H4PE_UIMSG     = 1 << 24,
    H4PE_HEARTBEAT = 0x80000000,
    H4PE_ALL       = 0xffffffff,
    H4PE_ALMOST_ALL= H4PE_ALL &~ H4PE_HEARTBEAT
};

enum H4P_UI_TYPE {
    H4P_UI_TEXT,
    H4P_UI_GPIO,
    H4P_UI_BOOL,
    H4P_UI_INPUT,
    H4P_UI_IMGBTN,
    H4P_UI_DROPDOWN,
    H4P_UI_IMG
};

using   H4P_NVP_MAP      = std::unordered_map<std::string,std::string>;
extern  H4P_PROXY_MAP   h4pGlobal;
extern  std::string     h4pSrc;

using H4P_FN_EVENTHANDLER   = std::function<void(const std::string& svc,H4PE_TYPE t,const std::string& msg)>;
using H4P_EVENT_LISTENER    = std::pair<std::string,H4P_FN_EVENTHANDLER>;
using H4P_EVENT_LISTENERS   = std::vector<H4P_EVENT_LISTENER>;
using H4P_EVENT_HANDLERS    = std::map<uint32_t,H4P_EVENT_LISTENERS>;
using H4P_FN_USEREVENT      = std::function<void(const std::string &msg)>;
using H4P_FN_VB             = std::function<void(bool)>;

using H4P_FN_UIGET          = std::function<std::string(void)>;

void h4pregisterhandler(const std::string& svc,uint32_t t,H4P_FN_EVENTHANDLER f);
void h4punregisterhandler(const std::string& svc,uint32_t t);
void h4pevent(const std::string& svc,H4PE_TYPE t,const std::string& msg="");
void h4pOnEvent(H4PE_TYPE t,H4P_FN_USEREVENT f);

#if SANITY
void h4pInventory();
#endif

extern H4P_EVENT_HANDLERS   h4pevt;
//
extern std::string h4pGetErrorMessage(uint32_t e);
extern std::string h4pGetEventName   (H4PE_TYPE e);
extern std::string h4pGetTaskType    (uint32_t e);
extern std::string h4pGetTaskName    (uint32_t e);
extern void   h4pClearEvent     (H4PE_TYPE e);
//
[[noreturn]] void    h4pFactoryReset();
[[noreturn]] void    h4pReboot();

std::string  h4preplaceparams(const std::string& s);

using 	H4_FN_MSG 		=std::function<uint32_t(std::vector<std::string>)>;

struct command{
	uint32_t            owner;
	uint32_t 		    levID;
	H4_FN_MSG 			fn;
};

using 	H4P_CMDMAP		    =std::unordered_multimap<std::string,command>;
using 	H4P_CMDMAP_I        =H4P_CMDMAP::iterator;

enum H4_CMD_ERROR:uint32_t  {
    H4_CMD_OK,
    H4_CMD_UNKNOWN,
    H4_CMD_TOO_FEW_PARAMS,
    H4_CMD_TOO_MANY_PARAMS,
    H4_CMD_NOT_NUMERIC,
    H4_CMD_OUT_OF_BOUNDS,
    H4_CMD_NAME_UNKNOWN,
    H4_CMD_PAYLOAD_FORMAT,
    H4_CMD_NOT_NOW
};

enum trustedIds {
    H4P_TRID_THRB=50,
    H4P_TRID_PATN,
    H4P_TRID_PP1x,
    H4P_TRID_PWM1,
    H4P_TRID_DBNC,
    H4P_TRID_RPTP,
    H4P_TRID_POLL,
    H4P_TRID_HOTA,
    H4P_TRID_MQMS,
    H4P_TRID_MQRC,
    H4P_TRID_REST,
    H4P_TRID_SOAP,
    H4P_TRID_UDPS,
    H4P_TRID_NTFY,
    H4P_TRID_SCMD,
    H4P_TRID_HLOG,
    H4P_TRID_QLOG,
    H4P_TRID_BTTO,
    H4P_TRID_IPPD,
    H4P_TRID_DALY,
    H4P_TRID_SHOT,
    H4P_TRID_SSET,
    H4P_TRID_SYNC,
    H4P_TRID_TIME,
    H4P_TRID_GATE,
    H4P_TRID_BOOT,
    H4P_TRID_D0,
    H4P_TRID_D1,
    H4P_TRID_D2,
    H4P_TRID_D3,
    H4P_TRID_UPKT,
    H4P_TRID_D5,
    H4P_TRID_D6,
    H4P_TRID_D7,
    H4P_TRID_D8,
    H4P_TRID_D9
};

enum H4PC_CMD_ID {
    H4PC_ROOT,
    H4PC_H4,
    H4PC_SHOW,
    H4PC_SVC,
    H4PC_MAX
};

enum H4P_SIG_SCHEME {
    H4P_SIG_STOP,
    H4P_SIG_PIN,
    H4P_SIG_PWM,
    H4P_SIG_PATTERN,
    H4P_SIG_MORSE,
    H4P_SIG_THROB,
    H4P_SIG_PULSE
};

class H4Service;
extern std::unordered_map<std::string,H4Service*> h4pmap;

template <typename T>
T* h4puncheckedcall(const std::string& svc){ return h4pmap.count(svc) ? static_cast<T*>(h4pmap[svc]):nullptr; }

template<typename... Args>
void h4psysevent(const std::string& svc,H4PE_TYPE t,const std::string& fmt, Args... args){
    char* buff=static_cast<char*>(malloc(H4PE_BUFFER+1));
    snprintf(buff,H4PE_BUFFER,CSTR(fmt),args...);
    h4pevent(svc,t,buff);
    free(buff);
}

void h4puiAdd(const std::string& n,H4P_UI_TYPE t,std::string h="u",const std::string& v="",uint8_t c=0);
void h4puiSync(const std::string& n,const std::string& v="");

std::string h4pGetErrorMessage(uint32_t e);
std::string h4pGetEventName   (H4PE_TYPE e);
std::string h4pGetLedColor    (uint8_t c);
std::string h4pGetTaskType    (uint32_t e);
std::string h4pGetTaskName    (uint32_t e);
void        h4pClearEvent     (H4PE_TYPE e);

#define ON true
#define OFF false

#define H4PAYLOAD vs.back()
#define H4PAYLOAD_INT atoi(CSTR(vs.back()))
#define STOI(n) atoi(CSTR(n))
#define PARAM_INT(n) STOI(vs[n])
#define CHOP_FRONT(vs) (std::vector<std::string>(++vs.begin(),vs.end()))
#define CMD(x) ([this](std::vector<std::string>)->uint32_t{ x(); return H4_CMD_OK; })
#define CMDNULL ([this](std::vector<std::string>)->uint32_t{ return H4_CMD_OK; })
#define CMDVS(x) ([this](std::vector<std::string> vs)->uint32_t{ return x(vs); })
#define VSCMD(x) uint32_t x(std::vector<std::string>)

#define QEVENT(e) h4pevent(_me,e)
#define XEVENT(e,f,...) h4psysevent(_me,e,f,__VA_ARGS__)
#define YEVENT(e,f) h4pevent(_me,e,f)

#if H4P_LOG_MESSAGES
    #define QLOG(f) h4pevent(_me,H4PE_MSG,f)
    #define XLOG(f,...) h4psysevent(_me,H4PE_MSG,f,__VA_ARGS__)
    #define SLOG(f,...) h4psysevent(h4pTag(),H4PE_MSG,f,__VA_ARGS__)
    #define h4pUserEvent(f,...) h4psysevent(userTag(),H4PE_MSG,f,__VA_ARGS__)
    #define h4pUIMessage(f,...) h4psysevent(userTag(),H4PE_UIMSG,f,__VA_ARGS__)
#else
    #define QLOG(f)
    #define XLOG(f,...)
    #define SLOG(f,...)
    #define h4pUserEvent(f,...)
#endif

#define SYSFATAL(f,...) h4psysevent(_me,H4PE_SYSFATAL,f,__VA_ARGS__)
#if H4P_SYS_LEVEL > 1
    #define SYSWARN(f,...) h4psysevent(_me,H4PE_SYSWARN,f,__VA_ARGS__)
    #define SYSINFO(f,...) h4psysevent(_me,H4PE_SYSINFO,f,__VA_ARGS__)
#else
    #define SYSWARN(f,...)
    #if H4P_SYS_LEVEL > 0
        #define SYSINFO(f,...) h4psysevent(_me,H4PE_SYSINFO,f,__VA_ARGS__)
    #else
        #define SYSINFO(f,...)
    #endif
#endif

extern H4P_CMDMAP h4pCmdMap;

template<typename T>
T* require(const std::string& svc){
    T* dll=h4puncheckedcall<T>(svc);
    if(!dll){
        dll=new T;
        dll->_init();
        dll->_filter&=~H4PE_BOOT; // we already done it!
    }
    return dll;
}

class H4Service {
                void                _envoi(const std::string& s);
                std::string         _uniquify(const std::string& name,uint32_t uqf=0);
                void                _sysHandleEvent(const std::string& svc,H4PE_TYPE t,const std::string& msg);
    protected:
                std::string         _me;
                uint32_t            _pid;

                void                _addLocals(H4P_CMDMAP local);
                std::vector<uint32_t>    _expectInt(std::string pl,const char* delim=",");
                uint32_t            _guardInt1(std::vector<std::string> vs,std::function<void(uint32_t)> f);
                uint32_t            _guardString2(std::vector<std::string> vs,std::function<H4_CMD_ERROR(std::string,std::string)> f);
                uint32_t            _guard1(std::vector<std::string> vs,H4_FN_MSG f);
        virtual void                _handleEvent(const std::string& svc,H4PE_TYPE t,const std::string& msg){}
        virtual void                _reply(std::string msg) { Serial.println(CSTR(msg)); }

        template<typename T>
        T* depend(const std::string& svc){
            _filter |= H4PE_SERVICE;
            h4pregisterhandler(_me,H4PE_SERVICE,[=](const std::string& s,H4PE_TYPE t,const std::string& m){ _sysHandleEvent(s,t,m); });
            _parent=svc;
            return require<T>(svc);
        }

        template<typename... Args>
        void reply(const char* f, Args... args){
            char* buff=static_cast<char*>(malloc(H4P_REPLY_BUFFER+1));
            snprintf(buff,H4P_REPLY_BUFFER,f,args...);
            _envoi(buff);
            free(buff);
        }
    public:
//      for now
                uint32_t            _filter=H4PE_NOOP;
                std::string         _parent="";
                bool                _running=false;
//
        H4Service(const std::string& name,uint32_t events=H4PE_NOOP,bool singleton=true): _filter(events | H4PE_BOOT | H4PE_STAGE2){
            _me=_uniquify(name);
            if(_me==name || (!singleton)){
                _pid=h4pmap.size()+H4PC_MAX;
                h4pmap[_me]=this;
                h4pregisterhandler(_me,_filter,[this](const std::string& s,H4PE_TYPE t,const std::string& m){ _sysHandleEvent(s,t,m); });
            } 
            else SYSWARN("Reload %s",CSTR(_me));
        }
#if H4P_LOG_MESSAGES
        virtual void                info();
#endif
//      syscall only
        virtual void                _init(){}
        virtual void                svcDown(){ YEVENT(H4PE_SERVICE,CSTR(stringFromInt(_running=false))); }
        virtual void                svcUp(){ YEVENT(H4PE_SERVICE,CSTR(stringFromInt(_running=true))); }
};

#define H4P_DEFAULT_SYSTEM_HANDLER case H4PE_SYSFATAL: \
    Serial.printf("\n************** FATAL ERROR ***********************\n*\n"); \
    Serial.printf("*    %s %s \n",CSTR(svc),CSTR(msg)); \ 
    Serial.printf("*\n**************************************************\n"); \
    pinMode(H4P_ASSUMED_LED,OUTPUT); while(1){ digitalWrite(H4P_ASSUMED_LED,LOW);delay(25);digitalWrite(H4P_ASSUMED_LED,HIGH);delay(25); }  \
case H4PE_SYSWARN: \
    Serial.printf("\n****************** WARNING ***********************\n*\n"); \
    Serial.printf("*    %s %s \n",CSTR(svc),CSTR(msg)); \ 
    Serial.printf("*\n**************************************************\n"); \
    break; \
case H4PE_SYSINFO: \
    Serial.printf("\n*INFO: %s %s\n",CSTR(svc),CSTR(msg)); \
    break;

#define H4PGLUE2(x,y) x##y
#define H4PGLUE3(x,y,z) x##y##z
#define H4PSVCUP(s) H4PGLUE3(on,s,Connect())
#define H4PSVCDOWN(s) H4PGLUE3(on,s,Disconnect())
#define H4PEVENTNAME(e) H4PGLUE2(H4PE_,e)
#define H4PEVENTCALL(e,s,m) H4PGLUE2(on,e)(s,m)
//
//      CONNECTORS
//
#define H4P_ONOFF_CONNECTOR(e) case H4PEVENTNAME(e): \
    h4p[stateTag()]=msg; \
    break;

#define H4P_ONOFF_CONNECTOR_INVERTED(e) case H4PEVENTNAME(e): \
    h4p.gvSetInt(stateTag(),(bool) !(STOI(msg))); \
    break;

#define H4P_TACT_BUTTON_CONNECTOR(x) if(svc==#x){ \
    if(STOI(msg)){\
        h4puiSync(#x,"1");\
        H4PGLUE3(on,x,Button()); \
        h4.once(175,[]{ \
            h4p.gvSetInt(#x,0);\
            h4puiSync(#x,"0");\
        });\
    }\
}

#define H4P_LATCH_BUTTON_CONNECTOR(x) if(svc==#x){ \
    if(svc==#x){\
        bool b=STOI(msg);\
        H4PGLUE3(on,x,Button(b)); \
        h4puiSync(#x,msg);\
    }\
}
//
//      ADAPTERS
//
#define H4P_SERVICE_ADAPTER(s) if(svc==lowercase(#s)){ \
    if(STOI(msg)) H4PSVCUP(s); \
    else H4PSVCDOWN(s); \
}
#define H4P_FUNCTION_ADAPTER(e) case H4PEVENTNAME(e): \
    H4PEVENTCALL(e,svc,msg); \
    break;

#define H4P_FUNCTION_ADAPTER_II(e) case H4PEVENTNAME(e): \
    H4PEVENTCALL(e,STOI(svc),STOI(msg)); \
    break;

#define H4P_FUNCTION_ADAPTER_SI(e) case H4PEVENTNAME(e): \
    H4PEVENTCALL(e,svc,STOI(msg)); \
    break;

#define H4P_FUNCTION_ADAPTER_VOID(e) case H4PEVENTNAME(e): \
    H4PGLUE2(on,e)(); \
    break;

#define H4P_FUNCTION_ADAPTER_GPIO H4P_FUNCTION_ADAPTER_II(GPIO)

#define H4P_FUNCTION_ADAPTER_PRESENCE H4P_FUNCTION_ADAPTER_SI(PRESENCE)