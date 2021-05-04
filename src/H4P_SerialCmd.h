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

#include<H4Service.h>

enum H4P_SVC_CONTROL {
    H4PSVC_RESTART,
    H4PSVC_STATE,
    H4PSVC_START,
    H4PSVC_STOP
};

class H4P_SerialCmd: public H4Service {        
        static  string          _fname; // cos rd/w are static :)

                VSCMD(_config);
                VSCMD(_get);
                VSCMD(_svcRestart);
                VSCMD(_svcInfo);
                VSCMD(_svcStart);
                VSCMD(_svcStop);

                H4P_CMDMAP_I    __exactMatch(const string& cmd,uint32_t owner);
                void            __flatten(function<void(string)> fn);

                void            _adjust(const string& name,int value);
                void            _createProxy(const string& name,bool save=false);
                uint32_t        _dispatch(vector<string> vs,uint32_t owner);
                void            _flattenCmds(function<void(string)> fn,string cmd="",string prefix="",uint32_t owner=0);
                void            _handleEvent(const string& svc,H4PE_TYPE t,const string& msg) override;
                void            _run();
                void            _showItem(const string& name){ reply("  %s %s=%s",(h4pGlobal[name]._save) ? "P":" ",CSTR(name),CSTR(h4pGlobal[name])); }
                uint32_t        _svcControl(H4P_SVC_CONTROL svc,vector<string> vs);
    protected:
                void            svcUp() override;
                void            svcDown() override;
    public:
        VSCMD(_dump);   // public so logger can use it
        static  string          _dumpTask(task*);

        H4P_SerialCmd(bool autoStop=false);

                h4proxy&        operator[](const string& name) {
                    _createProxy(name);
                    return h4pGlobal[name];
                }

                void            gvDec(const string& name);
                void            gvErase(const string& name){ gvErase({CSTR(name)}); }
                void            gvErase(initializer_list<const char*> nil);
                bool            gvExists(const string& name){ return h4pGlobal.count(name); }
                int             gvGetInt(const string& name);
                string          gvGetstring(const string& name);
                void            gvInc(const string& name);
                void            gvSave(const string& name){ gvSave({CSTR(name)}); }
                void            gvSave(initializer_list<const char*> sav);
                void            gvSetInt(const string& name,int value,bool save=false);
                void            gvSetstring(const string& name,const string& value,bool save=false);
//
                void            addCmd(const string& name,uint32_t owner, uint32_t levID,H4_FN_MSG f=nullptr);
                void            clear();
                void            help();

#if H4P_LOG_MESSAGES
                void            all();
                void            heap(){ reply("Heap=%u",_HAL_freeHeap()); } // NEEDS TO BE HAL'd
                void            info() override;
                void            plugins();
                void            showFS();
                void            showQ();
#endif
                uint32_t        invokeCmd(string,string="",const char* src=userTag());
                uint32_t        invokeCmd(string,uint32_t,const char* src=userTag()); 
        static  string          read(const string& fn);
                void            removeCmd(const string& name,uint32_t pid=0);
        static  uint32_t        write(const string& fn,const string& data,const char* mode="w");
//      syscall only
                uint32_t        _executeCmd(string topic, string pload);
                void            _init() override;
        static  void            _persist();
                uint32_t        _simulatePayload(string flat,const char* src=cmdTag());
};

extern H4P_SerialCmd h4p;