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
#include<H4P_SerialCmd.h>
#include<H4P_AsyncWebserver.h>

constexpr char* uuTag(){ return "Update URL"; }

#if H4P_USE_WIFI_AP
void H4P_WiFi::startAP(){
    static DNSServer* _dns53=nullptr;
    string opts;

    Serial.printf("H4P_WiFi::startAP DISCO mode=%d\n",WiFi.getMode());
    WiFi.disconnect(false,false); // prb 32 only: abandon curretn blank attempt or no scan possible
    Serial.printf("H4P_WiFi::startAP POST  mode=%d\n",WiFi.getMode());
//    WiFi.enableSTA(false); // force AP only
//    Serial.printf("H4P_WiFi::startAP DISCO mode=%d\n",WiFi.getMode());
    int n=WiFi.scanNetworks();
    if(n>0){
        for (uint8_t i = 0; i < n; i++){
            string ss=CSTR(WiFi.SSID(i));
            Serial.printf("Found %s\n",CSTR(ss));
            opts+=ss+"="+ss+",";
        }
        opts.pop_back();
        _cb[ssidTag()]=CSTR(WiFi.SSID(0)); // ensure internal value if user never changes droplist
    } elsehttps://www.youtube.com/watch?v=1oKTdxYyJA8
    WiFi.scanDelete();
//    WiFi.enableSTA(false); // force AP only

//    Serial.printf("UI add %s etc n=%d *%s* len=%d\n",ssidTag(),n,CSTR(opts),opts.size());
    h4asws._uiAdd(H4P_UIO_SSID,ssidTag(),H4P_UI_DROPDOWN,opts);
    h4asws._uiAdd(H4P_UIO_PSK,pskTag(),H4P_UI_INPUT);
    h4asws._uiAdd(H4P_UIO_DEVICE,deviceTag(),H4P_UI_INPUT,_cb[deviceTag()]);

    Serial.printf("UI add maybe name\n");
    if(isLoaded(upnpTag())) h4asws._uiAdd(H4P_UIO_NAME,nameTag(),H4P_UI_INPUT);

    Serial.printf("UI add maybe rupd\n");
    if(isLoaded(rupdTag())) h4asws._uiAdd(H4P_UIO_RUPD,uuTag(),H4P_UI_INPUT);

    Serial.printf("UI add maybe mqtt\n");
    if(isLoaded(mqttTag())){
        h4asws._uiAdd(H4P_UIO_MQB,brokerTag(),H4P_UI_INPUT);
        h4asws._uiAdd(H4P_UIO_MQP,portTag(),H4P_UI_INPUT);
        h4asws._uiAdd(H4P_UIO_MQU,mQuserTag(),H4P_UI_INPUT);
        h4asws._uiAdd(H4P_UIO_MQK,mQpassTag(),H4P_UI_INPUT);
    }

    Serial.printf("UI add always go\n");
    h4asws._uiAdd(H4P_UIO_GO4IT,"GO!",H4P_UI_BOOL,"",
        nullptr,
        [this](const string& b){
            h4.queueFunction([this](){
                if(isLoaded(mqttTag())){
                    _cb[mqconfTag()]=_cb[brokerTag()]+","+_cb[portTag()]+","+_cb[mQuserTag()]+","+_cb[mQpassTag()];
                    _save(mqconfTag());
                }
                _cb[rupdTag()]=_cb[uuTag()];
                _save(rupdTag());
                _save(deviceTag());
                _save(nameTag());
                change(_cb[ssidTag()],_cb[pskTag()]);
            });
    });
    WiFi.mode(WIFI_AP);
    H4EVENT("ENTER AP MODE %s MAC=%s",CSTR(_cb[deviceTag()]),CSTR(WiFi.softAPmacAddress()));
    show(); //WiFi.printDiag(Serial);
    WiFi.softAP(CSTR(_cb[deviceTag()]));
    _cb[ipTag()]=CSTR(WiFi.softAPIP().toString());//.c_str();
    _dns53=new DNSServer;
    _dns53->start(53, "*", WiFi.softAPIP());
    h4.every(100,[](){ _dns53->processNextRequest(); },nullptr,H4P_TRID_WFAP);

    h4asws.start(); // DONT run uphooks!
}
#endif