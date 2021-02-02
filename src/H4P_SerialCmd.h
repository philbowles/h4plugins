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

#include<H4PCommon.h>
#include<H4P_VerboseMessages.h>

enum H4P_SVC_CONTROL {
    H4PSVC_RESTART,
    H4PSVC_STATE,
    H4PSVC_START,
    H4PSVC_STOP
};
class H4P_SerialCmd: public H4Plugin {        
        VSCMD(_event);
        VSCMD(_svcRestart);
        VSCMD(_svcInfo);
        VSCMD(_svcStart);
        VSCMD(_svcStop);

                H4_CMD_MAP_I    __exactMatch(const string& cmd,uint32_t owner);
                void            __flatten(function<void(string)> fn);

                uint32_t        _dispatch(vector<string> vs,uint32_t owner);
                void            _flattenCmds(function<void(string)> fn,string cmd="",string prefix="",uint32_t owner=0);
                void            _hookIn() override;
                void            _run();        
                void            _start() override { h4._hookLoop([this](){ _run(); },_pid); H4Plugin::_start(); }
                void            _stop() override { h4._unHook(_pid); H4Plugin::_stop(); }
                uint32_t        _svcControl(H4P_SVC_CONTROL svc,vector<string> vs);
    public:
        VSCMD(_dump);   // public so logger can use it
        static  string          _dumpTask(task*);

        H4P_SerialCmd(bool autoStop=false);
        
                void            addCmd(const string& name,uint32_t owner, uint32_t levID,H4_FN_MSG f=nullptr);
                void            all();
                void            config(){ for(auto const& c:_cb) reply("%s=%s",CSTR(c.first),CSTR(c.second)); }        
                void            heap(){ reply("Heap=%u",ESP.getFreeHeap()); }        
                void            help();
                uint32_t        invokeCmd(string,string="",const char* src=userTag());
                uint32_t        invokeCmd(string,uint32_t,const char* src=userTag()); 
                void            plugins();
        static  string          read(const string& fn);
                void            removeCmd(const string& name,uint32_t _pid=0);
                void            showFS();
                void            showQ();
        static  uint32_t        write(const string& fn,const string& data,const char* mode="w");
//      syscall only
                uint32_t        _executeCmd(string topic, string pload);
                uint32_t        _simulatePayload(string flat,const char* src=cmdTag());
};
extern __attribute__((weak)) H4P_SerialCmd h4cmd;