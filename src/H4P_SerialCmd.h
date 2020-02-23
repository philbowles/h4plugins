/*
 MIT License

Copyright (c) 2019 Phil Bowles <H48266@gmail.com>
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

using   namespace std::placeholders;

#include<H4PCommon.h>
#ifndef ARDUINO_ARCH_STM32 
    #ifdef ARDUINO_ARCH_ESP8266
        #include<FS.h>
    #else
        #include<SPIFFS.h>
    #endif
#endif

class H4Plugin;

using H4P_FN_LOG = function<void(const string &msg,H4P_LOG_TYPE type,const string& source,const string& target)>;

class H4P_SerialCmd: public H4Plugin {
        vector<H4P_FN_LOG>  _logChain;
//  protected:
        VSCMD(_unload);

        H4_CMD_MAP_I    __exactMatch(const string& cmd,uint32_t owner);
        void            __flatten(function<void(string)> fn);

        uint32_t        _dispatch(vector<string> vs,uint32_t owner);
        void            _hookIn() override;
        void            _flattenCmds(function<void(string)> fn,string cmd="",string prefix="",uint32_t owner=0);
        string          _errorString(uint32_t e);

        void            run();
    public:
#ifndef ARDUINO_ARCH_STM32
        VSCMD(_dump);   // public so logger can use it
//      spiffs
        static  string   read(const string& fn);
        void             showSPIFFS();
        static  uint32_t write(const string& fn,const string& data,const char* mode="w");
#endif
        H4P_SerialCmd();
//      cmds
        void            all();
        void            config(){ for(auto const& c:_cb) H4Plugin::reply("%s=%s\n",CSTR(c.first),CSTR(c.second)); }        
        void            dumpQ();
        void            help();
        void            plugins();
        void            Qstats();
        void            tnames();
        void            showUnload();
//      user
        void            addCmd(const string& name,uint32_t owner, uint32_t levID,H4_FN_MSG f=nullptr){ _addCmd(name, {owner,levID,f}); }
        uint32_t        invokeCmd(string,string="",const char* src=userTag());			
        uint32_t        invokeCmd(string,uint32_t,const char* src=userTag()); 
        void            logEventType(H4P_LOG_TYPE,const string& fmt,...);
        void            removeCmd(const string& name,uint32_t subid=0); 
        void            unload(const uint32_t subid);
//      syscall only
        void            _addCmd(const string& name,struct command cmd){ commands.insert(make_pair(name,cmd)); }
//        void            _noOP(){} // for non-events
        void            _hookLogChain(H4P_FN_LOG f){ _logChain.push_back(f); }
        void            _logEvent(const string &msg,H4P_LOG_TYPE type,const string& source,const string& target);
        uint32_t        _executeCmd(string topic, string pload);
        uint32_t        _simulatePayload(string flat,const char* src="kybd");
};

extern __attribute__((weak)) H4P_SerialCmd h4sc;
 
#endif // H4SerialCmd_H