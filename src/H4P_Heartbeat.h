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
#if H4P_UI_HEALTH
    #include<H4P_EmitHeap.h>
    #include<H4P_EmitQ.h>
    #include<H4P_EmitLoopCount.h>
#else
    #include<H4P_EmitTick.h>
#endif
#include<H4P_Signaller.h>
#include<H4P_WiFi.h>
class H4P_Heartbeat: public H4Service {
#ifdef H4P_ASSUMED_LED
                H4_TIMER    _hbLED;
                size_t      _period=0;
#endif
                void        _handleEvent(const std::string& svc,H4PE_TYPE t,const std::string& msg) override;
                void        _run();
    public: 
#if H4P_UI_HEALTH
        explicit H4P_Heartbeat(): H4Service("beat",H4PE_VIEWERS | H4PE_HEARTBEAT | H4PE_HEAP | H4PE_LOOPS | H4PE_Q){
            require<H4P_EmitHeap>(heapTag());
            require<H4P_EmitQ>("Q");
            require<H4P_EmitLoopCount>("LPS");
#else
        explicit H4P_Heartbeat(): H4Service("beat",H4PE_VIEWERS | H4PE_HEARTBEAT){
#endif
            require<H4P_EmitTick>(tickTag());
            depend<H4P_WiFi>(wifiTag());
        }
#ifdef H4P_ASSUMED_LED
        H4P_Heartbeat(size_t period): // ASSUMED ONLY
            _period(period),
            H4Service("beat",H4PE_VIEWERS | H4PE_HEARTBEAT){
            require<H4P_EmitTick>(tickTag());
            depend<H4P_WiFi>(wifiTag());
        }
#endif
#if H4P_LOG_MESSAGES
                void        info() override { H4Service::info(); reply(" upTime=%s",CSTR(upTime())); }
#endif
        static  std::string secsToTime(uint32_t sex);
        static  std::string upTime(){ return h4p.gvGetstring(upTimeTag()); }
//
#ifdef H4P_ASSUMED_LED
                void        svcDown() override{
                    if(_period) h4.cancel(_hbLED);
                    H4Service::svcDown();
                }
                void        svcUp() override{
                    if(_period) _hbLED=h4.every(_period,[=]{ H4P_Signaller::signal(H4P_SIG_PULSE,"50"); }); // p = pulse pin
                    H4Service::svcUp();
                }
#endif
};