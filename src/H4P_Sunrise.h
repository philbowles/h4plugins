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
#ifndef H4P_Sunrise_H
#define H4P_Sunrise_H

#include<H4PCommon.h>
#include<H4P_WiFiSelect.h>
#include<H4P_Timekeeper.h>

enum H4P_EPHEMERA {
    SUNRISE,
    SUNSET
};

class H4P_Sunrise: public H4Plugin {
                string      _latlong;

                string      _ephTime(H4P_EPHEMERA e){ return _cb[e==SUNRISE ? "sunrise":"sunset"]; }
                void        _hookIn() override;
                void        _greenLight() override {}; // no autostart
                void        _parse(const string& s);
                void        _setupSNTP(const string& ntp1, const string& ntp2);
                void        _start() override;
    public:
        H4P_Sunrise(const string& latitude,const string& longitude): H4Plugin("sset"){ _latlong="lat="+latitude+"&lng="+longitude; }

                void        at(H4P_EPHEMERA riseOrSet,bool onoff,H4BS_FN_SWITCH f){ h4tk.at(_ephTime(riseOrSet),onoff,f); }
                void        atSource(H4P_EPHEMERA riseOrSet,bool onoff){ h4tk.atSource(_ephTime(riseOrSet),onoff); }
//                void        daily(H4P_EPHEMERA riseOrSet,bool onoff,H4BS_FN_SWITCH f){ h4tk.daily(_ephTime(riseOrSet),onoff,f); }
//                void        dailySource(H4P_EPHEMERA riseOrSet,bool onoff){ h4tk.dailySource(_ephTime(riseOrSet),onoff); }
                void show() override {
                    reply("Loc=%s",CSTR(_latlong));
                    reply("Rise@%s Set@%s",CSTR(_cb["sunrise"]),CSTR(_cb["sunset"]));
                }
};

extern __attribute__((weak)) H4P_Sunrise h4ss;

#endif // H4P_Sunrise_H