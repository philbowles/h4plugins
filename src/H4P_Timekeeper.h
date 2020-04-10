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
#ifndef H4P_Timekeeper_H
#define H4P_Timekeeper_H

#include<H4PCommon.h>
#ifdef ARDUINO_ARCH_ESP8266
#include "sntp.h"

class H4P_Timekeeper: public H4Plugin {
        VSCMD(_at);
        VSCMD(_daily);

                uint32_t    _mss00=0;
                int         _tzo;
                string      _ntp1,_ntp2;

                uint32_t    __alarmCore (vector<string> vs,bool daily);
               ip_addr_t*  __ntpSetServer(int n,const char* ntp);

        virtual void        _hookIn() override;
        virtual void        _greenLight() override {}; // no autostart
                uint32_t    _msDue(const char* rtc);
        virtual void        _start() override;
        virtual void        _stop() override;
                void        _useNTP(int offset,const char* srv1,const char* srv2);
    public:
        H4P_Timekeeper(const char* ntp1,const char* ntp2,int tzOffset=0,H4_FN_VOID onStart=nullptr,H4_FN_VOID onStop=nullptr);

                H4_TIMER    at(const char* when,H4_FN_VOID f,H4_FN_VOID fnc=nullptr,uint32_t id=0);        
                string 		clockTime() { return _mss00 ? strTime(msSinceMidnight() / 1000) : "0"; }
                H4_TIMER    daily(const char* when,H4_FN_VOID f,H4_FN_VOID fnc=nullptr,uint32_t id=0);        
                string		getDate(){ return _cb["date"]; };
//                long 		getRawTimestamp() { return sntp_get_current_timestamp(); }
                bool 		hasRTC() { return _mss00; }
                uint32_t 	msSinceMidnight() { return _mss00 + millis(); }
                uint32_t 	parseTime(const string& ts);
                void        show() override { reply("TZO=%d\ntp1=%s\nntp2=%s\nUpTime: %s",_tzo,CSTR(_ntp1),CSTR(_ntp2),CSTR(upTime())); }
//                uint32_t 	secSinceMidnight() { return msSinceMidnight() / 1000; }
                string 		strTime(uint32_t t);
                void        sync();
                string 		upTime();
};

extern __attribute__((weak)) H4P_Timekeeper h4tk;

#endif // esp8266 only
#endif // H4P_Timekeeper_H
