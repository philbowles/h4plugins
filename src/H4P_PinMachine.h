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
#include<H4P_SerialCmd.h>

using H4PM_STAGE_MAP = std::vector<uint32_t>;

class h4pGPIO;
using H4P_PINMAP = std::map<uint8_t,h4pGPIO*>;
extern H4P_PINMAP h4pPinMap;

struct msg {
    uint8_t         p;
    unsigned long   time;
    uint32_t        delta;
    int             load;
    uint32_t        phy;
    bool            halt;
    bool            timer;
    uint32_t        sigE;
};

using H4P_MSG_FLOW      = std::function<msg(msg)>;

class npNODE {
    public:
        virtual msg operator()(msg m){};
        static  msg publish(msg m,int v,H4PE_TYPE t=H4PE_GPIO);
};

using npFLOW  = std::vector<npNODE*>;

using H4PM_COMPARE=std::function<bool(uint32_t,uint32_t)>;
#define H4PM_LESS std::less<uint32_t>()
#define H4PM_GREATER std::greater<uint32_t>()

class h4pGPIO {
    friend  class H4P_PinMachine;
        virtual void                _handleEvent(const std::string& s,H4PE_TYPE t,const std::string& msg);
    protected:
                uint32_t            _r; // to cope with analog values
//
        virtual void                _announce();
                uint32_t inline     _normalise(uint32_t x){ return isAnalogInput() ? x:!(x^_s); }
        virtual void                _syncValue(){ 
                    _r=digitalRead(_p);
                    _prev={_p,0,0,_normalise(_r),_r,false,false,0};
                }
    public:
//      system use for efficieny: DO NOT TOUCH THEM!!!
                msg                 _prev;
                uint8_t             _c=H4P_UILED_BI; // color
                uint8_t             _p;
                npFLOW              _pipeline;
                H4PM_SENSE          _s;
//
        h4pGPIO(uint8_t p,uint8_t m,H4PM_SENSE s=ACTIVE_LOW,npFLOW flow={});
#if H4P_LOG_MESSAGES
                std::string              dump();
#endif
                int                 getValue(){ return _prev.load; }
                msg                 inject(uint32_t metal,bool timer=false);
        virtual bool                isAnalogInput(){ return _HAL_isAnalogInput(_p); }
        virtual bool                isOutput(){ return false; }
                uint8_t             logicalRead(){ return _normalise(_r); }
                uint8_t             pin(){ return _p; }
                msg                 runFlow(msg seed);
};
//
//      H4P_PinMachine
//
class H4P_PinMachine: public H4Service {
                void            _run();
    public:
        H4P_PinMachine(): H4Service(gpioTag()) {}
#if H4P_LOG_MESSAGES
        static  msg             dump(uint8_t p);
                void            info() override;
#endif
        static  void            all(bool b);
        static  uint8_t         logicalRead(uint8_t p);
        static  void            logicalWrite(uint8_t p,bool b);
        static  int             getValue(uint8_t p);
        static  msg             inject(uint8_t p,uint32_t metal,bool timer=false);
        static  bool            isAnalogInput(uint8_t p);
        static  h4pGPIO*        isManaged(uint8_t p){ return h4pPinMap.count(p) ? h4pPinMap[p]:nullptr; }
        static  bool            isOutput(uint8_t p);
        static  H4_TIMER        periodicRead(uint8_t p,uint32_t f);
        static  msg             runFlow(uint8_t p,msg);
// syscall only
                void            svcUp() override;
                void            svcDown() override;

        template <typename T,typename F, typename... Args>
        static  T               delegate(uint8_t p, F f,Args... args) {
            auto ptr = H4P_PinMachine::isManaged(p);
            return ptr ? (ptr->*f)(args...):T{};
        }
};

#include<NODE-PINK-NODES.h>
#include<NODE-PINK-FLOWS.h>