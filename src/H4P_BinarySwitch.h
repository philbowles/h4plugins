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

#include<H4P_BinaryThing.h>
#include<H4P_PinMachine.h>

class H4P_BinarySwitch: public H4P_BinaryThing{
            uint8_t         _pin;
            H4PM_SENSE      _sense;
            uint32_t        _initial;      
    protected:
        h4pOutput*          _pp;
    public:
            uint8_t         _color;

        H4P_BinarySwitch(uint8_t pin,H4PM_SENSE sense,uint8_t color=H4P_UILED_BI,uint32_t initial=OFF,uint32_t timer=0):
            _pin(pin),
            _sense(sense),
            _initial(initial),
            _color(color),
            H4P_BinaryThing([=](bool b){ _pp->logicalWrite(b); },initial,timer){ require<H4P_PinMachine>(gpioTag()); }
//
        virtual void        _init() override;
};

class H4P_ConditionalSwitch: public H4P_BinarySwitch{
                H4_FN_CPRED _predicate;
    protected:
        virtual void        _handleEvent(const std::string& svc,H4PE_TYPE t,const std::string& msg) override;
    public:
        H4P_ConditionalSwitch(H4_FN_CPRED predicate,uint8_t pin,H4PM_SENSE sense,uint8_t color=H4P_UILED_BI,uint32_t initial=OFF,uint32_t timer=0):
            _predicate(predicate), 
            H4P_BinarySwitch(pin,sense,color,initial,timer){ syncCondition(); }
#if H4P_LOG_MESSAGES
        void info() override { 
            H4P_BinarySwitch::info();
            reply(" Condition %d",_predicate());
        }
#endif
        void syncCondition();
};