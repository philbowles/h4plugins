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
#ifndef H4P_Heartbeat_H
#define H4P_Heartbeat_H

#include<H4PCommon.h>

class H4P_Heartbeat: public H4Plugin {
        static  uint32_t    _uptime;
 
                H4_FN_VOID  _bf;

                void        _greenLight() override { _start(); }; // autostart
                void        _start() override;
                void        _stop() override ;
                void        _run();
    public: 
        H4P_Heartbeat(H4_FN_VOID beat): _bf(beat),H4Plugin(beatTag()){}

        static string secsToTime(uint32_t sex);

        void show() override { reply("upSecs=%u %s\n",upSecs(),CSTR(upTime())); }

        static uint32_t upSecs(){ return _uptime; }

        static string upTime(){ return secsToTime(_uptime); }
};

#endif // H4P_Heartbeat_H
