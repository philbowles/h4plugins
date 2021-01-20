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
#include<H4P_WiFi.h>

uint32_t            H4P_Heartbeat::_uptime=0;

void H4P_Heartbeat::_handleEvent(const string &msg,H4P_EVENT_TYPE type,const string& source) {
    _uptime=STOI(msg);
    if(_f) _f(_uptime);
};

void H4P_Heartbeat::_hookIn() {
    DEPEND(tick);
    if(isLoaded(wifiTag())){
        if(WiFi.getMode()==WIFI_STA) h4wifi._uiAdd(H4P_UIO_UP,"uptime",H4P_UI_LABEL,"",upTime);
    }
    H4PEventListener::_hookIn();
}

string H4P_Heartbeat::secsToTime(uint32_t sex){ 
    char buf[15];
    sprintf(buf,"%3d %02d:%02d:%02d",sex/86400,(sex%86400)/3600,(sex/60)%60,sex%60);
    return string(buf);
}
