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
#ifndef H4SerialCmd_H
#define H4SerialCmd_H

#include<H4PCommon.h>
using  namespace std::placeholders;

enum H4P_SVC_CONTROL {
    H4PSVC_RESTART,
    H4PSVC_STATE,
    H4PSVC_START,
    H4PSVC_STOP
};

using H4P_FN_LOG = function<void(const string &msg,H4P_LOG_TYPE type,const string& source,const string& target)>;

extern void h4FactoryReset();
class H4P_SerialCmd: public H4Plugin {
        vector<H4P_FN_LOG>  _logChain;
        
        VSCMD(_svcRestart);
        VSCMD(_svcInfo);
        VSCMD(_svcStart);
        VSCMD(_svcStop);

        H4_CMD_MAP_I    __exactMatch(const string& cmd,uint32_t owner);
        void            __flatten(function<void(string)> fn);

        uint32_t        _dispatch(vector<string> vs,uint32_t owner);
        void            _hookIn() override;
        void            _flattenCmds(function<void(string)> fn,string cmd="",string prefix="",uint32_t owner=0);
        string          _errorString(uint32_t e);
        uint32_t        _svcControl(H4P_SVC_CONTROL svc,vector<string> vs);

        void            _run();        
        void            _greenLight(){ start(); }
        void            _start() override {
            h4._hookLoop([this](){ _run(); },_subCmd);
            H4Plugin::_start();
            Serial.printf("H4P Version %s\n",CSTR(_cb[h4PvTag()]));
        }
        void            _stop() override {
            h4._unHook(_subCmd);
            H4Plugin::_stop();
        }
    public:
        H4P_SerialCmd(bool autoStop=false);
        static  string          read(const string& fn);
        static  uint32_t        write(const string& fn,const string& data,const char* mode="w");
                void            showFS();
                void            heap(){ reply("Heap=%u",ESP.getFreeHeap()); }        
                void            all();
                void            config(){ for(auto const& c:_cb) reply("%s=%s",CSTR(c.first),CSTR(c.second)); }        
                void            plugins();

        VSCMD(_dump);   // public so logger can use it

                void            showQ();
        static  string          _dumpTask(task*);
//      user
                void            addCmd(const string& name,uint32_t owner, uint32_t levID,H4_FN_MSG f=nullptr){  _commands.insert(make_pair(name,command {owner,levID,f})); }
                void            help();
                uint32_t        invokeCmd(string,string="",const char* src=userTag());			
                uint32_t        invokeCmd(string,uint32_t,const char* src=userTag()); 
                void            removeCmd(const string& name,uint32_t _subCmd=0); 

                template<typename... Args>
                void            logEventType(H4P_LOG_TYPE t,const string& src,const string& tgt,const string& fmt, Args... args){
                    char* buff=static_cast<char*>(malloc(256)); // fix this!
                    snprintf(buff,255,CSTR(fmt),args...);
                    _logEvent(buff,t,src,tgt);
                    free(buff);
                }

//      syscall only
                void            _hookLogChain(H4P_FN_LOG f){ _logChain.push_back(f); }
                void            _logEvent(const string &msg,H4P_LOG_TYPE type,const string& source,const string& target);
                uint32_t        _executeCmd(string topic, string pload);
                uint32_t        _simulatePayload(string flat,const char* src=scmdTag());
};

extern __attribute__((weak)) H4P_SerialCmd h4cmd;

#endif // H4SerialCmd_H