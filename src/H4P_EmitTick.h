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

#include<H4PCommon.h>
#include<H4P_SerialCmd.h>

class H4P_EmitTick: public H4Plugin {
            uint32_t    _uptime;
            bool        _skip=false;

                void    _run() {
                    uint32_t now=millis();
                    uint32_t nowsec=now/1000;
                    if(!(now%1000) && nowsec!=_uptime) {
                        _uptime=nowsec;
                        if(!_skip) PEVENT(H4P_EVENT_HEARTBEAT,"%u",_uptime);
//                        else PLOG("BEAT SKIPPED");
                        _skip=false;
                    }
                }
                void _handleEvent(H4PID pid,H4P_EVENT_TYPE t,const string& msg) { _skip=true; }
    public: 
        H4P_EmitTick(): H4Plugin(H4PID_1SEC,H4P_EVENT_BACKOFF){}

                void    show() override {
                    H4Plugin::show();
                    reply("Uptime %u",_uptime);
                }
                void    _hookIn() override { h4._hookLoop([this](){ _run(); },_pid); }
};