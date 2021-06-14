/*
 MIT License

Copyright (c) 2020 Phil Bowles <H48266@gmail.com>
   github     https://github.com/philbowles/H4
   blog       https://8266iot.blogspot.com
   groups     https://www.facebook.com/groups/esp8266questions/
              https://www.facebook.com/H4-Esp8266-Firmware-Support-2338535503093896/

Portions (c) 2020 Adam Sharp http://www.threeorbs.co.uk/

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
#ifdef ARDUINO_ARCH_ESP8266
    #include "sntp.h"
#else
    #include "lwip/apps/sntp.h"
    #include <time.h>
#endif

#include<H4P_WiFi.h>

using H4P_DURATION = std::pair<std::string,std::string>;
using H4P_SCHEDULE = std::vector<H4P_DURATION>;
/* e.g.
H4P_SCHEDULE mySchedule={
  {"07:00","09:00"}, // ON @ 0700, OFF @ 0900
  {"13:00","14:00"}, // ON @ 1300, OFF @ 1400
  {"18:00","23:00"}  // ON @ 1800, OFF @ 2300
};
*/

using H4_FN_DST        =std::function<int(uint32_t)>;

class H4P_Timekeeper: public H4Service {
        VSCMD(_change);
        VSCMD(_tz);
                H4_FN_DST           _fDST;
                std::string         _ntp1;
                std::string         _ntp2;
                uint32_t            _mss00=0;
                struct tm           _timeinfo;
                int                 _tzo;

                uint32_t    __alarmCore (std::vector<std::string> vs,bool daily);
                void        __HALsetTimezone(int);
                uint32_t    _at(std::vector<std::string> vs);
                uint32_t    _daily(std::vector<std::string> vs);
                void        _handleEvent(const std::string& svc,H4PE_TYPE t,const std::string& msg) override;
                void        _setupSNTP(const std::string& ntp1, const std::string& ntp2);

    public:
        H4P_Timekeeper(const std::string& ntp1,const std::string& ntp2,int tzOffset=0,H4_FN_DST fDST=nullptr);
///
// _tzo = Time Zone Offset in +/- minutes from UTC, not hours.
                uint32_t	clockEPOCH() { time_t now = 0; return _mss00 ? time(&now) : 0; }  // Current number of seconds since EPOCH.
                uint32_t	clockEPOCHLocal() { return clockEPOCH() + (_tzo * 60); }  // Current number of seconds since EPOCH plus TZ offset.
                std::string clockStrTimeLocal() { return strTime( (clockEPOCHLocal() % 86400) * 1000 ); }  // Current time + TZ as std::string HH:MM:SS.
                std::string strfTime(uint32_t t);
                std::string strfDate(uint32_t t);
                std::string strfDateTime(char fmt[] = "%a %Y-%m-%d %H:%M:%S", uint32_t t=0);
        static  int 		H4P_DST_EU(uint32_t t);  // DST offset for t in EU (inc UK).
        static  int 		H4P_DST_USA(uint32_t t);  // DST offset for t in USA.
///
                void        at(const std::string& when,bool onoff);
                void        change(const std::string& ntp1,const std::string& ntp2);
                std::string clockTime() { return _mss00 ? strTime(msSinceMidnight()) : "0"; }
                void        daily(const std::string& when,bool onoff);
                uint32_t 	msSinceMidnight() { return _mss00 + millis(); }
                int      	parseTime(const std::string& ts);
                std::string minutesFromNow(uint32_t s);
                void        setSchedule(H4P_SCHEDULE);
#if H4P_LOG_MESSAGES
                void        info() override;
#endif
                std::string strTime(uint32_t t);
                void        sync();
                void        tz(int tzOffset);
                std::string upTime();
// syscall only
        virtual void        _init() override;
        virtual void        svcUp() override;
        virtual void        svcDown() override;
};