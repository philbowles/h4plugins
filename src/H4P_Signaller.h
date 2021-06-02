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
#include<H4P_PinMachine.h>

#ifndef H4P_ASSUMED_LED
    #define H4P_ASSUMED_LED      LED_BUILTIN
    #define H4P_ASSUMED_SENSE     ACTIVE_LOW
    #define H4P_ASSUMED_COLOR H4P_UILED_BLUE
#endif

class H4Flasher{
	        std::string          _dots;
            H4_TASK_PTR     _timer=nullptr;
            H4_TASK_PTR	    _off=nullptr;
            h4pOutput*      _opp;

            uint32_t        _duty=0;
            std::string          _pattern;
            uint32_t        _period=0;
            uint32_t        _timebase=0;
            uint32_t        _valley=0;

            void      		_pulse(uint32_t width);	
    public:  
        H4Flasher(h4pOutput*  opp,uint32_t period,uint8_t duty);
        H4Flasher(h4pOutput*  opp,const char* pattern,uint32_t timebase);

                void        flashPattern();
                void        PWM();
                void        stop();
#ifdef ARDUINO_ARCH_ESP8266
        H4Flasher(h4pOutput*  opp,uint32_t period,uint32_t valley);
                void        throb();
#endif
//      syscall
#if H4P_LOG_MESSAGES
                std::string      _dump(){
                    char* buff=static_cast<char*>(malloc(H4P_REPLY_BUFFER+1));
                    snprintf(buff,H4P_REPLY_BUFFER,"D/P/V %d,%d,%d P/T '%s',%d",_duty,_period,_valley,CSTR(_pattern),_timebase);
                    std::string rv(buff);
                    free(buff);
                    return rv;
                }
#endif
};

using H4FC_FN_F1        = std::function<void(H4Flasher*)>;
using H4FC_FN_F2        = std::function<H4Flasher*(h4pOutput*)>;
using H4P_FLASHMAP      = std::unordered_map<uint8_t,H4Flasher*>;

extern H4P_FLASHMAP h4pFlashMap;
class H4P_Signaller: public H4Service {     
            void            _dynaLoad(uint8_t pin,H4PM_SENSE active,uint8_t col,H4FC_FN_F1 f1,H4FC_FN_F2 f2);
            void            _flash(uint32_t period,uint8_t duty,uint8_t pin,H4PM_SENSE active=ACTIVE_HIGH,uint8_t col=H4P_ASSUMED_COLOR);
#if H4P_CMDLINE_FLASHERS
            uint32_t        __validator(vector<std::string> vs,H4_FN_MSG f);
            VSCMD(_morse);
            VSCMD(_onof);
            VSCMD(_patn);
            VSCMD(_pwm);
            VSCMD(_stop);
#endif
    protected:
        virtual void        _handleEvent(const std::string& svc,H4PE_TYPE t,const std::string& msg);
    public:
        H4P_Signaller(): H4Service(winkTag(),H4PE_SIGNAL){
            require<H4P_PinMachine>(gpioTag());
            _running=true; // earliest possible start
#if H4P_CMDLINE_FLASHERS
            _addLocals({
                {_me,       { H4PC_H4, _pid, nullptr }}, 
                {"morse",   { _pid, 0, CMDVS(_morse) }},
                {"onof",    { _pid, 0, CMDVS(_onof) }},
                {"patn",    { _pid, 0, CMDVS(_patn) }},
                {"pwm",     { _pid, 0, CMDVS(_pwm) }},
                {stopTag(), { _pid, 0, CMDVS(_stop) }}
            });
#endif
        }
             
            void 			flashMorse(const char *pattern, uint32_t timebase, uint8_t pin,H4PM_SENSE active=H4P_ASSUMED_SENSE,uint8_t col=H4P_ASSUMED_COLOR);	
            void 			flashMorse(const char *pattern, uint32_t timebase,h4pOutput*);	
#ifdef H4FC_MORSE_SUPPORT
            static std::unordered_map<char,string> _morse; // tidy
            void 	        flashMorseText(const char * pattern,uint32_t timebase,uint8_t pin,H4PM_SENSE active=H4P_ASSUMED_SENSE,uint8_t col=H4P_ASSUMED_COLOR);
            void 	        flashMorseText(const char * pattern,uint32_t timebase,h4pOutput*);
#endif
            void 			flashPattern(const char * pattern,uint32_t timebase,uint8_t pin,H4PM_SENSE active=H4P_ASSUMED_SENSE,uint8_t col=H4P_ASSUMED_COLOR);
            void 			flashPattern(const char * pattern,uint32_t timebase,h4pOutput*);
            void 			flashPin(uint32_t rate, uint8_t pin,H4PM_SENSE active=ACTIVE_HIGH,uint8_t col=H4P_ASSUMED_COLOR);
            void 			flashPin(uint32_t rate,h4pOutput*);
            void 			flashPWM(uint32_t period,uint8_t duty,uint8_t pin,H4PM_SENSE active=H4P_ASSUMED_SENSE,uint8_t col=H4P_ASSUMED_COLOR);	
            void 			flashPWM(uint32_t period,uint8_t duty,h4pOutput*);	
#if H4P_LOG_MESSAGES
            void            info() override;
#endif
            bool 			isFlashing(uint8_t pin);
            void 			pulsePin(uint32_t period,uint8_t pin,H4PM_SENSE active=H4P_ASSUMED_SENSE,uint8_t col=H4P_ASSUMED_COLOR);
            void 			pulsePin(uint32_t period,h4pOutput*);
            void            stopAll();
            void            stopPin(uint8_t pin);
            void            stopPin(h4pOutput*);
#ifdef ARDUINO_ARCH_ESP8266
            void 			throbPin(uint32_t rate, uint32_t valley, uint8_t pin,H4PM_SENSE active=H4P_ASSUMED_SENSE,uint8_t col=H4P_ASSUMED_COLOR);
            void 			throbPin(uint32_t rate, uint32_t valley,h4pOutput*);
#endif
//          syscall only
};