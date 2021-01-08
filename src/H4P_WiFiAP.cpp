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
#include<H4PCommon.h>
#include<H4P_WiFi.h>
#include<H4P_AsyncWebServer.h>

//diag
#include<H4P_SerialCmd.h>

void H4P_WiFiAP::_start(){
    auto mode=WiFi.getMode();
    H4EVENT("APMODE _mcuStart mode=%d stored",mode);
    if(mode==WIFI_AP) startAP();
    else {
        if(WiFi.SSID()==""){
            H4EVENT("APMODE NO CONFIG: Cannot Allow auto reconnect");
            startAP();
        }
        else {
            if(mode==WIFI_STA) { H4EVENT("STA configured %s, let it run",CSTR(WiFi.SSID())); }
            else {
                H4EVENT("must be OFF, but config for %s so force STA",CSTR(WiFi.SSID()));
                HAL_WIFI_startSTA();
            }
        }
    }
}

void H4P_WiFiAP::startAP(){
    static DNSServer* _dns53=nullptr;

    WiFi.enableSTA(true);

    int n=WiFi.scanNetworks();
    string opts;
    for (uint8_t i = 0; i < n; i++){
        string ss=CSTR(WiFi.SSID(i));
        opts+=ss+"="+ss+",";
    }
    opts.pop_back();
    _cb[ssidTag()]=CSTR(WiFi.SSID(0)); // ensure internal value if user never changes droplist
    WiFi.scanDelete();

    h4asws._uiAdd(-1,ssidTag(),H4P_UI_DROPDOWN,opts);
    h4asws._uiAdd(0,pskTag(),H4P_UI_INPUT);
    h4asws._uiAdd(1,deviceTag(),H4P_UI_INPUT,_cb[deviceTag()]); 
    if(isLoaded(upnpTag())) h4asws._uiAdd(2,nameTag(),H4P_UI_INPUT,"",[]{ return _cb[nameTag()]; }); // cos we don't know it yet
    h4asws._uiAdd(3,"Connect...",H4P_UI_BOOL,"",
        nullptr,
        [this](const string& b){
            h4.queueFunction([this](){
                H4EVENT("APAPAP User config connect to %s/%s",CSTR(_cb[ssidTag()]),CSTR(_cb[pskTag()]));
                H4EVENT("APAPAP device / friendly %s/%s",CSTR(_cb[deviceTag()]),CSTR(_cb[nameTag()]));
//                setBothNames(_cb[deviceTag()],_cb[nameTag()]);
                _save(deviceTag());
                _save(nameTag());
                change(_cb[ssidTag()],_cb[pskTag()]);
            });
    });

    WiFi.enableSTA(false); // force AP only

    WiFi.mode(WIFI_AP);
    H4EVENT("ENTER AP MODE %s MAC=%s",CSTR(_cb[deviceTag()]),CSTR(WiFi.softAPmacAddress()));
    WiFi.softAP(CSTR(_cb[deviceTag()]));
    _cb[ipTag()]=CSTR(WiFi.softAPIP().toString());//.c_str();
    _dns53=new DNSServer;
    _dns53->start(53, "*", WiFi.softAPIP());
    h4.every(100,[](){ _dns53->processNextRequest(); },nullptr,H4P_TRID_WFAP);

    h4asws.start(); // DONT run uphooks!
}
