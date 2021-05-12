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
#include<H4Service.h>
#include<H4P_WiFi.h>

#if H4P_USE_WIFI_AP
void H4P_WiFi::_startAP(){
    h4p.gvSetInt(GoTag(),0,false);

    YEVENT(H4PE_SIGNAL,"250,.  "); 
    string opts=string(" Select SSID...").append(UNIT_SEPARATOR).append("dummy").append(RECORD_SEPARATOR);
    _dns53=new DNSServer;

    HAL_WIFI_disconnect();

    int n=WiFi.scanNetworks();
    if(n>0){
        for (uint8_t i = 0; i < n; i++){
            string ss=CSTR(WiFi.SSID(i));
            opts+=ss+UNIT_SEPARATOR+ss+RECORD_SEPARATOR;
        }
        opts.pop_back();
    }
    WiFi.scanDelete();

    _uiAdd(ssidTag(),H4P_UI_DROPDOWN,"s",opts);
    _uiAdd(pskTag(),H4P_UI_INPUT,"s");
    _uiAdd(deviceTag(),H4P_UI_INPUT,"s");
    _uiAdd(GoTag(),H4P_UI_IMGBTN,"o");

    WiFi.mode(WIFI_AP);
    SYSINFO("ENTER AP MODE %s MAC=%s",CSTR(h4p[deviceTag()]),CSTR(WiFi.softAPmacAddress()));

    WiFi.softAP(CSTR(h4p[deviceTag()]));
    _dns53->start(53, "*", WiFi.softAPIP());
    h4.every(H4WF_AP_RATE,[=](){ _dns53->processNextRequest(); });

    _startWebserver();
}
#endif