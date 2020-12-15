/*
 MIT License

Copyright (c) 2020 Phil Bowles <h4plugins@gmail.com>
   github     https://github.com/philbowles/esparto
   blog       https://8266iot.blogspot.com     
   groups     https://www.facebook.com/groups/esp8266questions/
              https://www.facebook.com/Esparto-Esp8266-Firmware-Support-2338535503093896/
                			  

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
#include<H4P_Heartbeat.h>

uint32_t            H4P_Heartbeat::_uptime=0;

void H4P_Heartbeat::_start() {
    h4._hookLoop([this](){ _run(); },_subCmd);
    H4Plugin::_start();
}

void H4P_Heartbeat::_stop() {
    h4._unHook(_subCmd);
    H4Plugin::_stop();
}

void H4P_Heartbeat::_run(){
    uint32_t now=millis();
    uint32_t nowsec=now/1000;
    if(!(now%1000) && nowsec!=_uptime) {
        _bf();
        _uptime=nowsec;
    }
}

string H4P_Heartbeat::secsToTime(uint32_t sex){ 
    char buf[15];
    sprintf(buf,"%3d %02d:%02d:%02d",sex/86400,(sex%86400)/3600,(sex/60)%60,sex%60);
    return string(buf);
}
