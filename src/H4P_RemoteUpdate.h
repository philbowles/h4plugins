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
#ifndef H4P_RemoteUpdate_H
#define H4P_RemoteUpdate_H

#include<H4PCommon.h>
#include<H4P_SerialCmd.h>

#ifdef ARDUINO_ARCH_ESP8266
    #include<ESP8266httpUpdate.h>
class H4P_RemoteUpdate: public H4Plugin, public ESP8266HTTPUpdate {
#else
    #include<HTTPUpdate.h>
class H4P_RemoteUpdate: public H4Plugin, public HTTPUpdate {
#endif
                H4P_WiFi*   _pWiFi;
                WiFiClient  _c;
                string      _url;
                void        _entropise(H4_FN_VOID f){ h4.onceRandom(H4P_PJ_LO,H4P_PJ_HI * H4P_RUPD_STRETCH,f); }
                void        _greenLight(){
                    if(_pWiFi->_getPersistentValue(rupdTag(),"")) _url=_cb[rupdTag()];
                    _cb[rupdTag()]=httpTag()+_url; 
                } // no autostart
                void        _handleEvent(H4PID pid,H4P_EVENT_TYPE t,const string& msg) override { _pWiFi->_wipe(rupdTag()); }
                void        _hookIn(){ _pWiFi=h4pdepend<H4P_WiFi>(this,H4PID_WIFI); }
                void        _updateFromUrl(bool fw,bool reboot){
                    string endpoint=_cb[rupdTag()]+"/"+_cb[deviceTag()];
                    t_httpUpdate_return rv=fw ? update(_c,CSTR(endpoint)):updateSpiffs(_c,CSTR(endpoint));
                    switch(rv){
                        case HTTP_UPDATE_OK:
                            if(reboot) h4reboot();
                            break;
                        case HTTP_UPDATE_NO_UPDATES:
                            PLOG("Already up to date");
                            break;
                        default:
                            PLOG("FAIL %d: %s", getLastError(), getLastErrorString().c_str());
                    }
                }
    public:
 #if H4P_USE_WIFI_AP
        H4P_RemoteUpdate(const char* bin): H4Plugin(H4PID_RUPD){
#else
        H4P_RemoteUpdate(const string& url,const char* bin): _url(url),  H4Plugin(H4PID_RUPD,H4P_EVENT_FACTORY){
#endif
            _cb["bin"]=bin;
            _addLocals({
                {_pName,       { H4PC_H4,_pid  , nullptr}},
                {"both",       { _pid, 0, CMD(both)}},
                {"fs",         { _pid, 0, CMD(fs)}},
                {"fw",         { _pid, 0, CMD(fw)}}
            });
        }

        void both(){
            _entropise([this]{ _updateFromUrl(false,false); }); // spiffs, no reboot
            fw(); // fw + reboot
        }
        void show() override { reply("url: %s",CSTR(_cb[rupdTag()])); }
        void fs(){ _entropise([this]{ _updateFromUrl(false,true); }); }
        void fw(){ _entropise([this]{ _updateFromUrl(true,true); }); }
};

#endif // H4P_RemoteUpdate_H
