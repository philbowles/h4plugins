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

STAG(beat);

class H4P_Heartbeat: public H4Plugin {
        static  uint32_t    _uptime;
 
                function<void(H4P_Heartbeat*)> _bf;

                void        _greenLight() override { _start(); }; // autostart
                void        _start() override {
                    h4._hookLoop([this](){ _run(); },_subCmd);
                    H4Plugin::_start();
                }
                void        _stop() override {
                    h4._unHook(_subCmd);
                    H4Plugin::_stop();
                }
                void _run(){
                    uint32_t now=millis();
                    uint32_t nowsec=now/1000;
                    if(!(now%1000) && nowsec!=_uptime) {
                        _bf(this);
                        _uptime=nowsec;
                    }
                }
    public:
        H4P_Heartbeat(function<void(H4P_Heartbeat*)> beat): _bf(beat),H4Plugin("beat"){}
        
        static string secsToTime(uint32_t sec){ 
            Serial.printf("secsToTime %d\n",sec);
            char* t="000d 00:00:00";
            uint32_t s=sec%60;
            Serial.printf("secsToTime s=%d\n",s);
            uint32_t m=sec/60;
            Serial.printf("secsToTime m=%d\n",m);
            uint32_t h=sec/3600;
            Serial.printf("secsToTime h=%d\n",h);
            uint32_t d=sec/86400;
            Serial.printf("secsToTime d=%d\n",d);
            sprintf(t,"%3dd %02d:%02d:%02d",d,h,m,s);
            Serial.printf("secsToTime FINAL %s\n",t);
            return string(t);
        }

        static uint32_t upSecs(){ return _uptime; }

        static string upTime(){ return secsToTime(_uptime); }
};

#endif // H4P_Heartbeat_H
