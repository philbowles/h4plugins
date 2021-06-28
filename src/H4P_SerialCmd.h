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
                VSCMD(_config);
                VSCMD(_get);
                VSCMD(_svcRestart);
                VSCMD(_svcInfo);
                VSCMD(_svcStart);
                VSCMD(_svcStop);

                H4P_CMDMAP_I    __exactMatch(const std::string& cmd,uint32_t owner);
                void            __flatten(std::function<void(std::string)> fn);

                void            _adjust(const std::string& name,int value);
                void            _createProxy(const std::string& name,bool save=false);
                uint32_t        _dispatch(std::vector<std::string> vs,uint32_t owner);
                void            _flattenCmds(std::function<void(std::string)> fn,std::string cmd="",std::string prefix="",uint32_t owner=0);
                void            _handleEvent(const std::string& svc,H4PE_TYPE t,const std::string& msg) override;
                void            _run();
                void            _showItem(const std::string& name){ reply("  %s %s=%s",(h4pGlobal[name]._save) ? "P":" ",CSTR(name),CSTR(h4pGlobal[name])); }
                uint32_t        _svcControl(H4P_SVC_CONTROL svc,std::vector<std::string> vs);
    protected:
                void            svcUp() override;
                void            svcDown() override;
    public:
        VSCMD(_dump);   // public so logger can use it

        H4P_SerialCmd(bool autoStop=false);

                h4proxy&        operator[](const std::string& name) {
                    _createProxy(name);
                    return h4pGlobal[name];
                }

                void            gvDec(const std::string& name);
                void            gvErase(const std::string& name){ gvErase({CSTR(name)}); }
                void            gvErase(std::initializer_list<const char*> nil);
                bool            gvExists(const std::string& name){ return h4pGlobal.count(name); }
                int             gvGetInt(const std::string& name);
                std::string     gvGetstring(const std::string& name);
                void            gvInc(const std::string& name);
                void            gvSave(const std::string& name){ gvSave({CSTR(name)}); }
                void            gvSave(std::initializer_list<const char*> sav);
                void            gvSetInt(const std::string& name,int value,bool save=false);
                void            gvSetstring(const std::string& name,const std::string& value,bool save=false);
//
                void            addCmd(const std::string& name,uint32_t owner, uint32_t levID,H4_FN_MSG f=nullptr);
                void            clear();
                void            help();

#if H4P_LOG_MESSAGES
                void            all();
                void            heap(){ reply("Heap=%u",_HAL_freeHeap()); } // NEEDS TO BE HAL'd
                void            info() override;
                void            plugins();
                void            showFS();
    #if H4_HOOK_TASKS
        static  void            showQ(){ H4::dumpQ(); };
    #endif
#endif
                uint32_t        invokeCmd(std::string,std::string="",const char* src=userTag());
                uint32_t        invokeCmd(std::string,uint32_t,const char* src=userTag()); 
        static  std::string     read(const std::string& fn);
                void            removeCmd(const std::string& name,uint32_t pid=0);
        static  uint32_t        write(const std::string& fn,const std::string& data,const char* mode="w");
//      syscall only
                uint32_t        _executeCmd(std::string topic, std::string pload);
        static  void            _persist();
                uint32_t        _simulatePayload(std::string flat,const char* src=cmdTag());
};

extern H4P_SerialCmd h4p;