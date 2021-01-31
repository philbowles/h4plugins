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
#include<H4Utils.h>

#include"config.h"

#ifdef ARDUINO_ARCH_ESP8266
    #include<LittleFS.h>
    #define HAL_FS LittleFS
#else
    #include<FS.h>
    #include<SPIFFS.h>
    #define HAL_FS SPIFFS
#endif

#include<unordered_set>
#include<map>
//
#ifndef ARDUINO_SONOFF_BASIC // or s20 / sv ect
  #define RELAY_BUILTIN   12
  #define BUTTON_BUILTIN  0
#endif
//
// literal string RAM savers
//
#define STAG(x) constexpr const char* x##Tag(){ return #x; }

constexpr const char* cmdhash(){ return "/h4/#"; }

STAG(auto);
STAG(board)
STAG(broker);
STAG(chip);
STAG(device);
STAG(gpio);
STAG(h4);
STAG(h4UI);
STAG(ip);
STAG(mqconf);
STAG(mQpass);
STAG(mqtt);
STAG(mQuser);
STAG(name);
STAG(onof);
STAG(pmv);
STAG(port);
STAG(psk); // chg password
STAG(rupd);
STAG(cmd);
STAG(src);
STAG(ssid);
STAG(state);
STAG(upnp);
STAG(user);
STAG(wifi);
//
// evt 2, son of
//
enum H4PID {
    H4PID_SYS, // ROOT
    H4PID_H4, // ROOT
    H4PID_SHOW, // ROOT
    H4PID_SVC, // ROOT
    H4PID_VM, // 1
    H4PID_CMD, // 2
    H4PID_1SEC, // 3
    H4PID_ESQW,
    H4PID_EVTQ, // 5
    H4PID_HEAP,
    H4PID_HEAR,
    H4PID_HWRN,
    H4PID_LLOG,
    H4PID_LOOP, // 10
    H4PID_QWRN,
    H4PID_SNIF,
    H4PID_STOR, // 15
    H4PID_TONE,
    H4PID_GPIO, // 17
    H4PID_WINK, // 18
    H4PID_WIFI, // 19
    H4PID_BEAT, // 20
    H4PID_MQTT, // 21
    H4PID_ONOF,
    H4PID_UPNP,
    H4PID_GANG,
    H4PID_MFNB,
    H4PID_PDIP, // 25
    H4PID_PDMD,
    H4PID_PDUP,
    H4PID_RUPD,
    H4PID_SQLL,
    H4PID_SSET, // 30
    H4PID_TIME,
    H4PID_UDPL,
    H4PID_UNKNOWN
};

enum H4P_EVENT_TYPE:uint32_t {
    H4P_EVENT_NOOP,
    H4P_EVENT_MSG       = 1 <<  0,
    H4P_EVENT_SVC_UP    = 1 <<  1,
    H4P_EVENT_SVC_DOWN  = 1 <<  2,
    H4P_EVENT_CMD       = 1 <<  3,
    H4P_EVENT_USER      = 1 <<  4,
    H4P_EVENT_HEAP      = 1 <<  5,
    H4P_EVENT_Q         = 1 <<  6,
    H4P_EVENT_LOOPS     = 1 <<  7,
    H4P_EVENT_PD_ENTER  = 1 <<  8,
    H4P_EVENT_PD_LEAVE  = 1 <<  9,
    H4P_EVENT_MQTT_ERROR= 1 << 10,
    H4P_EVENT_REBOOT    = 1 << 11,
    H4P_EVENT_FACTORY   = 1 << 12,
    H4P_EVENT_CMDREPLY  = 1 << 13,
    H4P_EVENT_DLL       = 1 << 14,
    H4P_EVENT_ON        = 1 << 15,
    H4P_EVENT_OFF       = 1 << 16,
    H4P_EVENT_UISYNC    = 1 << 17,
    H4P_EVENT_HEARTBEAT=0x80000000,
    H4P_EVENT_ALL=0x7fffffff
};

using H4P_CONFIG_BLOCK      = unordered_map<string,string>;

using H4P_FN_EVENTHANDLER   = function<void(H4PID pid,H4P_EVENT_TYPE t,const string& msg)>;
using H4P_EVENT_LISTENER    = pair<H4PID,H4P_FN_EVENTHANDLER>;
using H4P_EVENT_LISTENERS   = vector<H4P_EVENT_LISTENER>;
using H4P_EVENT_HANDLERS    = unordered_map<uint32_t,H4P_EVENT_LISTENERS>;
using H4P_FN_USEREVENT      = function<void(const string &msg)>;

void h4pregisterhandler(H4PID pid,uint32_t t,H4P_FN_EVENTHANDLER f);
void h4pemit(H4PID pid,H4P_EVENT_TYPE t,const char* msg);
void h4pOnEvent(H4P_EVENT_TYPE t,H4P_FN_USEREVENT f);

extern H4P_EVENT_HANDLERS   h4pevt;
extern H4P_CONFIG_BLOCK     _cb;

template<typename... Args>
void h4psysevent(H4PID pid,H4P_EVENT_TYPE t,const string& fmt, Args... args){
    char* buff=static_cast<char*>(malloc(H4P_EVENT_BUFFER+1));
    snprintf(buff,H4P_EVENT_BUFFER,CSTR(fmt),args...);
    h4pemit(pid,t,buff);
    free(buff);
}
//
string h4pgetErrorMessage(uint32_t e);
string h4pgetEventName   (H4P_EVENT_TYPE e);
string h4pgetTaskType    (uint32_t e);
string h4pgetTaskName    (uint32_t e);
//
void h4FactoryReset(const string& src=userTag());

string h4pGetConfig(const string& c);
int h4pGetConfigInt(const string& c);
void h4pSetConfig(const string& c,const string& v);
void h4pSetConfig(const string& c,const int v);

string h4preplaceparams(const string& s);

#if SANITY
void h4psanitycheck();
#endif

using 	H4_FN_MSG 		=function<uint32_t(vector<string>)>;
using   H4BS_FN_SWITCH  =function<void(bool)>;

struct command{
	uint32_t            owner;
	uint32_t 		    levID;
	H4_FN_MSG 			fn;
};

using 	H4_CMD_MAP		    =std::unordered_multimap<string,command>;
using 	H4_CMD_MAP_I        =H4_CMD_MAP::iterator;
using   H4P_FN_PSCHANGE     =function<void(const string&,const string&)>;

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

#define ON true
#define OFF false

#define H4PAYLOAD vs.back()
#define H4PAYLOAD_INT atoi(CSTR(vs.back()))
#define STOI(n) atoi(CSTR(n))
#define PARAM_INT(n) STOI(vs[n])
#define CHOP_FRONT(vs) (vector<string>(++vs.begin(),vs.end()))
#define CMD(x) ([this](vector<string>)->uint32_t{ x(); return H4_CMD_OK; })
#define CMDNULL ([this](vector<string>)->uint32_t{ return H4_CMD_OK; })
#define CMDVS(x) ([this](vector<string> vs)->uint32_t{ return x(vs); })
#define VSCMD(x) uint32_t x(vector<string>)
#define QTHIS(f) h4.queueFunction([this]{ (f)(); })

enum trustedIds {
  H4P_TRID_PATN = 50,
  H4P_TRID_PP1x,
  H4P_TRID_PWM1,
  H4P_TRID_DBNC,
  H4P_TRID_RPTP, // 55
  H4P_TRID_POLL,
  H4P_TRID_MULT,
  H4P_TRID_TRIG,
  H4P_TRID_HOTA, // 60
  H4P_TRID_WFAP,
  H4P_TRID_MQMS,
  H4P_TRID_MQRC,
  H4P_TRID_REST,
  H4P_TRID_SOAP, // 65
  H4P_TRID_UDPS,
  H4P_TRID_UDPU,
  H4P_TRID_NTFY,
  H4P_TRID_SCMD, // 70
  H4P_TRID_HLOG,
  H4P_TRID_QLOG,
  H4P_TRID_MLRQ,
  H4P_TRID_BTTO,
  H4P_TRID_IPPD, // 75
  H4P_TRID_TIME,
  H4P_TRID_SYNC,
  H4P_TRID_DALY,
  H4P_TRID_SHOT, // 80
  H4P_TRID_SSET
};

enum H4PC_CMD_ID {
    H4PC_ROOT,
    H4PC_H4,
    H4PC_SHOW,
    H4PC_SVC,
    H4PC_MAX
};

enum H4P_UI_ORDER:uint32_t {
    H4P_UIO_SSID,
    H4P_UIO_PSK,
    H4P_UIO_DEVICE,
    H4P_UIO_NAME,
    H4P_UIO_RUPD,
    H4P_UIO_CHIP,
    H4P_UIO_BOARD,
    H4P_UIO_IP,
    H4P_UIO_H4V,
    H4P_UIO_H4PV,
    H4P_UIO_H4UIV,
    H4P_UIO_UP,
    H4P_UIO_MQTT,
    H4P_UIO_PMV,
    H4P_UIO_MQB,
    H4P_UIO_MQP,
    H4P_UIO_MQU,
    H4P_UIO_MQK,
    H4P_UIO_GO4IT,
    H4P_UIO_USER,
    H4P_UIO_COND=900,
    H4P_UIO_ONOF,
    H4P_UIO_GPIO
};

class H4Plugin;
extern std::map<uint32_t,H4Plugin*> h4pmap;
extern vector<string> h4pnames;

#if H4P_LOG_EVENTS
    #define SEVENT(t,f,...) h4psysevent(H4PID_SYS,t,f, ##__VA_ARGS__ )
    #define SLOG(f,...) h4psysevent(H4PID_SYS,H4P_EVENT_MSG,f, ##__VA_ARGS__ )
    #define PEVENT(t,f,...) h4psysevent(_pid,t,f, ##__VA_ARGS__ )
    #define PLOG(f,...) h4psysevent(_pid,H4P_EVENT_MSG,f, ##__VA_ARGS__ )
    #define h4UserEvent(x,...) { h4psysevent(H4PID_H4,H4P_EVENT_USER,x, ##__VA_ARGS__); }
#else
    #define h4UserEvent(x,...)
    #define SEVENT(t,f,...)
    #define SLOG(f,...)
    #define PEVENT(t,f,...)
    #define PLOG(f,...)
#endif
H4Plugin* h4pptrfromtxt(const string& s);

class H4Plugin {
                vector<H4_FN_VOID>  _connected;
                vector<H4_FN_VOID>  _disconnected;
                bool                _up=false;
                
                void                _envoi(const string& s);
                uint32_t            _eventFilter=H4P_EVENT_NOOP;
    protected:
        static  H4_CMD_MAP          _commands;
                bool                _discoDone=false;

                void                _addLocals(H4_CMD_MAP local);
                vector<uint32_t>    _expectInt(string pl,const char* delim=",");  
                uint32_t            _guardInt1(vector<string> vs,function<void(uint32_t)> f);
                uint32_t            _guardString2(vector<string> vs,function<H4_CMD_ERROR(string,string)> f);
                uint32_t            _guard1(vector<string> vs,H4_FN_MSG f);
        virtual void                _handleEvent(H4PID pid,H4P_EVENT_TYPE t,const string& msg){}
                void                _init(H4PID pid,H4_FN_VOID svcUp=nullptr,H4_FN_VOID svcDown=nullptr);
        virtual void                _restart(){ stop();start(); }
        virtual void                _start() { _upHooks(); }
        virtual void                _stop() { _downHooks(); }
                string              _uniquify(const string& name,uint32_t uqf=0);
    public:
//        static  H4P_CONFIG_BLOCK    _cb;
                H4PID               _pid;
                string              _pName;
//
//      statics
//
        H4Plugin(const string name,uint32_t filter=H4P_EVENT_NOOP,H4_FN_VOID svcUp=nullptr,H4_FN_VOID svcDown=nullptr): _eventFilter(filter){
            string uniquename=_uniquify(name,1);
            H4PID pid=static_cast<H4PID>(h4pnames.size());
            h4pnames.push_back(uniquename);
            _init(pid,svcUp,svcDown);
        }

        H4Plugin(H4PID pid,uint32_t filter=H4P_EVENT_NOOP,H4_FN_VOID svcUp=nullptr,H4_FN_VOID svcDown=nullptr): _eventFilter(filter){ _init(pid,svcUp,svcDown); }
//
//        static  string      getConfig(const string& c){ return _cb[c]; }
//        static  int         getConfigInt(const string& c){ return STOI(_cb[c]); }
                void        hookConnect(H4_FN_VOID f){ _connected.push_back(f); }
                void        hookDisconnect(H4_FN_VOID f){ _disconnected.push_back(f); } 

        template<typename... Args>
        void                reply(const char* f, Args... args){
            char* buff=static_cast<char*>(malloc(H4P_REPLY_BUFFER+1));
            snprintf(buff,H4P_REPLY_BUFFER,f,args...);
            _envoi(buff);
            free(buff);
        }
                void        restart(){ _restart(); };
//        static  void        setConfig(const string& c,const string& v){ _cb[c]=v; }
//        static  void        setConfig(const string& c,const int v){ _cb[c]=stringFromInt(v); }
        virtual void        show(){}
                void        start();
                bool        state(){ return _state(); }
                void        stop();
//
//      syscall only
//
                void        _downHooks();
        virtual void        _greenLight(){ start(); } // override if necessary!
        virtual void        _hookIn(){}
        virtual void        _reply(string msg) { Serial.println(CSTR(msg)); }
        virtual bool        _state(){ return _up; }
                void        _upHooks();

//        static void         dumpCommands(H4_CMD_MAP cm=_commands){ for(auto const c:cm) Serial.printf("%16s o=%2d l=%2d\n",CSTR(c.first),c.second.owner,c.second.levID); }
};

template<typename T>
T* h4pisloaded(H4PID p){ return h4pmap.count(p) ? reinterpret_cast<T*>(h4pmap[p]):nullptr; }

//void h4pdll(H4Plugin* dll);

template<typename T,typename... Args>
T* h4prequire(H4Plugin* me,H4PID p,Args... args){
    T* test=h4pisloaded<T>(p);
    if(test) return test;
    auto dll=new T(args...);
    SEVENT(H4P_EVENT_DLL,"%s cascade load %s",CSTR(me->_pName),CSTR(h4pnames[p]));
//    h4pdll(dll);
    dll->_hookIn();
    return dll;
}

void h4pupcall(H4Plugin* me,H4Plugin* ptr);

template<typename T,typename... Args>
T* h4pdepend(H4Plugin* me,H4PID p,Args... args){
    SLOG("%s depends on %s",CSTR(me->_pName),CSTR(h4pnames[p]));
    T* ptr=h4prequire<T>(me,p,args...);
    h4pupcall(me,ptr);
    return ptr;
}