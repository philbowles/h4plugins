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
#ifndef H4P_HO
#define H4P_HO

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

bool stringIsAlpha(const string& s); // move to util!!!
//
#ifndef ARDUINO_SONOFF_BASIC // or s20 / sv ect
  #define RELAY_BUILTIN   12
  #define BUTTON_BUILTIN  0
#endif
//
using 	H4_FN_MSG 		=function<uint32_t(vector<string>)>;
using   H4BS_FN_SWITCH  =function<void(bool)>;

struct command{
	uint32_t            owner;
	uint32_t 		    levID;
	H4_FN_MSG 			fn;
};

using 	H4_CMD_MAP		    =std::unordered_multimap<string,command>;
using 	H4_CMD_MAP_I        =H4_CMD_MAP::iterator;
using   H4P_CONFIG_BLOCK    =std::unordered_map<string,string>;
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

enum H4P_LOG_TYPE {
    H4P_LOG_H4=1,
    H4P_LOG_SVC_UP=2,
    H4P_LOG_SVC_DOWN=4,
    H4P_LOG_CMD=8,
    H4P_LOG_USER=16,
    H4P_LOG_MQTT_HEAP=64,
    H4P_LOG_MQTT_Q=128,
    H4P_LOG_PD_ENTER=256,
    H4P_LOG_PD_LEAVE=512,
    H4P_LOG_MQTT_ERROR=1024,
    H4P_LOG_ALL=0xffffffff,
    H4P_LOG_ERROR=H4P_LOG_ALL
};

#define ON true
#define OFF false
//
// literal string RAM savers
//
#define STAG(x) constexpr char* x##Tag(){ return #x; }

constexpr const char* cmdhash(){ return "/h4/#"; }

//STAG(asws);
STAG(auto);
STAG(board)
STAG(broker);
STAG(chip);
STAG(device);
STAG(gpio);
STAG(h4);
STAG(h4UI);
STAG(ip);
STAG(log);
STAG(mqconf);
STAG(mQpass);
STAG(mqtt);
STAG(mQuser);
STAG(msg);
STAG(name);
STAG(onof);
STAG(pmv);
STAG(port);
STAG(psk); // chg password
STAG(rupd);
STAG(scmd);
STAG(snif);
STAG(src);
STAG(ssid);
STAG(state);
STAG(time);
STAG(upnp);
STAG(user);
STAG(vm);
STAG(wink);
STAG(wifi);

#define H4PAYLOAD vs.back()
#define H4PAYLOAD_INT atoi(CSTR(vs.back()))
#define STOI(n) atoi(CSTR(n))
#define PARAM_INT(n) STOI(vs[n])
#define CHOP_FRONT(vs) (vector<string>(++vs.begin(),vs.end()))
// synthesize CMD OK returner with null vec for simple void functions TODO CMDR(x) ?
#define CMD(x) ([this](vector<string>)->uint32_t{ x(); return H4_CMD_OK; })
#define CMDNULL ([this](vector<string>)->uint32_t{ return H4_CMD_OK; })
#define CMDVS(x) ([this](vector<string> vs)->uint32_t{ return x(vs); })
#define VSCMD(x) uint32_t x(vector<string>)
#define QTHIS(f) h4.queueFunction([this]{ (f)(); })

#ifdef H4P_LOG_EVENTS
    #define SYSEVENT(e,s,t,x,...) { h4cmd.logEventType(e,s,t,x, ##__VA_ARGS__); }
    #define H4EVENT(x,...) { h4cmd.logEventType(H4P_LOG_H4,_pName,h4Tag(),x, ##__VA_ARGS__); }
    #define DEPENDFAIL(x) { Serial.printf("FATAL: %s needs %s\n",CSTR(_pName),x##Tag());return; }
    #define h4UserEvent(x,...) { h4cmd.logEventType(H4P_LOG_USER,userTag(),h4Tag(),x, ##__VA_ARGS__); }
#else
    #define SYSEVENT(e,x,...)
    #define H4EVENT(x,...)
    #define DEPENDFAIL(x)
    #define h4UserEvent(x,...)
#endif

#define HOOK_IF_LOADED(x) { H4Plugin* p=isLoaded(x##Tag()); \
            if(p) { p->hookConnect([this](){ start(); }); p->hookDisconnect([this](){ H4Plugin::stop(); }); }}
#define DEPEND(x) { H4Plugin* p=isLoaded(x##Tag()); \
            if(p) { p->hookConnect([this](){ start(); });p->hookDisconnect([this](){ H4Plugin::stop(); }); }\
            else { DEPENDFAIL(x) } }
#define REQUIRE(x) { H4Plugin* p=isLoaded(x##Tag()); \
            if(!p) { DEPENDFAIL(x) } }
#define REQUIREBT { H4Plugin* p=isLoaded(onofTag()); \
            if(!p) { DEPENDFAIL(onof) } \
            _btp=reinterpret_cast<H4P_BinaryThing*>(p); }
//
enum trustedIds {
  H4P_TRID_PATN = 50,
  H4P_TRID_PP1x,
  H4P_TRID_PWM1,
  H4P_TRID_GPIO,
  H4P_TRID_DBNC,
  H4P_TRID_RPTP, // 55
  H4P_TRID_POLL,
  H4P_TRID_MULT,
  H4P_TRID_TRIG,
  H4P_TRID_SQWV,
  H4P_TRID_HOTA, // 60
  H4P_TRID_WFAP,
  H4P_TRID_MQMS,
  H4P_TRID_MQRC,
  H4P_TRID_REST,
  H4P_TRID_SOAP, // 65
  H4P_TRID_UDPM,
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
  H4P_TRID_LOOP,
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

class H4Plugin {
    protected:
        static  H4_CMD_MAP          _commands;
        static  uint32_t            _nxtSubCmd;
        static  H4P_CONFIG_BLOCK    _cb;

                H4_CMD_MAP          _cmds={};

                vector<H4_FN_VOID>  _connected;
                vector<H4_FN_VOID>  _disconnected;

                bool                _discoDone=false;
                bool                _up=false;

        vector<uint32_t>    expectInt(string pl,const char* delim=",");  
    
            uint32_t        guardInt1(vector<string> vs,function<void(uint32_t)> f);

            uint32_t        guardString2(vector<string> vs,function<H4_CMD_ERROR(string,string)> f);

            uint32_t        guard1(vector<string> vs,H4_FN_MSG f){
                if(!vs.size()) return H4_CMD_TOO_FEW_PARAMS;
                return vs.size()>1 ? H4_CMD_TOO_MANY_PARAMS:f(vs);
            }

        virtual void        _restart(){ stop();start(); }
        virtual void        _start() { _upHooks(); }
        virtual void        _stop() { _downHooks(); }
    public:
        static  vector<H4_FN_VOID>  _factoryChain;
                H4_FN_VOID          _factoryHook=[]{};
//                H4_FN_VOID          _rebootHook=[this]{ stop(); };
        static  vector<H4Plugin*>   _plugins;
                string              _pName;
                uint32_t            _subCmd;
//       
        virtual void        _greenLight(){ start(); } // override if necessary!
        virtual void        _hookIn(){}
        virtual bool        _state() { return _up; }

                void        _startup();
//
        H4Plugin(const string& name,H4_FN_VOID svcUp=nullptr,H4_FN_VOID svcDown=nullptr): _pName(name){
            _subCmd=++_nxtSubCmd;
            _plugins.push_back(this);
            if(svcUp) hookConnect(svcUp);
            if(svcDown) hookDisconnect(svcDown);
        };

        static H4Plugin*   isLoaded(const string& x){
            for(auto const& p:H4Plugin::_plugins) if(p->_pName==x) return p;
            return nullptr;
        }
        /*
        static string   pluginList(){
            string rv="[";
            for(auto const& p:H4Plugin::_plugins) rv+="\""+p->_pName+"\",";
            rv.pop_back();
            return rv+"]";
        }
        */
                void        restart(){ _restart(); };
        virtual void        show(){}
                void        start();
                bool        state(){ return _state(); }
                void        stop();
//
                void        hookConnect(H4_FN_VOID f){ _connected.push_back(f); }
                void        hookDisconnect(H4_FN_VOID f){ _disconnected.push_back(f); } 
//
        static  string      getConfig(const string& c){ return _cb[c]; }
        static  int         getConfigInt(const string& c){ return STOI(_cb[c]); }
        static  void        setConfig(const string& c,const string& v){ _cb[c]=v; }
        static  void        setConfig(const string& c,const int v){ _cb[c]=stringFromInt(v); }
                
                template<typename... Args>
                void        reply(const char* fmt, Args... args){
                    char* buff=static_cast<char*>(malloc(H4P_REPLY_BUFFER+1));
                    snprintf(buff,H4P_REPLY_BUFFER,fmt,args...);
                    string source=_cb[srcTag()];
                    H4Plugin* p=isLoaded(source);
                    if(p) p->_reply(buff);
                    else Serial.println(buff);
                    free(buff);
                }
//      syscall only
                void        _downHooks();
        virtual void        _reply(string msg) { Serial.println(CSTR(msg)); }
                void        _upHooks();

//        static void         dumpCommands(H4_CMD_MAP cm=_commands){ for(auto const c:cm) Serial.printf("%16s o=%2d l=%2d\n",CSTR(c.first),c.second.owner,c.second.levID); }
};

class H4PLogService: public H4Plugin {
                uint32_t    _filter=0;
    protected:
                void        _filterLog(const string &m,H4P_LOG_TYPE t,const string& s,const string& tg){ if(_up && (t & _filter)) _logEvent(m,t,s,tg); }
        virtual void        _hookIn() override;
        virtual void        _logEvent(const string &msg,H4P_LOG_TYPE type,const string& source,const string& target)=0;
    public:
        H4PLogService(const string& lid,uint32_t filter=0xffffffff): _filter(filter), H4Plugin(lid){}
        virtual void        show(){ reply("%s Filter 0x%08x\n",CSTR(_pName),_filter); }
};

#endif // H4P_HO