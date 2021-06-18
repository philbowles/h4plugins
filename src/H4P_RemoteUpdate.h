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

#include<H4Service.h>
#include<H4P_SerialCmd.h>

#ifdef ARDUINO_ARCH_ESP8266
    #include<ESP8266httpUpdate.h>
class H4P_RemoteUpdate: public H4Service, public ESP8266HTTPUpdate {
#else
    #include<HTTPUpdate.h>
class H4P_RemoteUpdate: public H4Service, public HTTPUpdate {
#endif
            WiFiClient  _c;
            std::string endpoint;
            void        _commonCTOR(){
                rebootOnUpdate(false);
                depend<H4P_WiFi>(wifiTag());
                _addLocals({
                    {_me,          { H4PC_H4,_pid  , nullptr}},
                    {"both",       { _pid, 0, CMD(both)}},
                    {"fs",         { _pid, 0, CMD(fs)}},
                    {"fw",         { _pid, 0, CMD(fw)}}
                });
            }
            void        _entropise(H4_FN_VOID f){ h4.onceRandom(H4P_PJ_LO,H4P_PJ_HI * H4P_RUPD_STRETCH,f); }

            void _handleEvent(const std::string& svc,H4PE_TYPE t,const std::string& msg) override {
                switch(t){
                    case H4PE_VIEWERS:
                        {
                            uint32_t mode=STOI(msg);
                            if(mode) {
                            #if H4P_USE_WIFI_AP
                                if(mode==WIFI_AP) h4puiAdd(rupdTag(),H4P_UI_INPUT,"s");
                                else h4puiAdd(rupdTag(),H4P_UI_TEXT,"s");
                            #else
                                h4puiAdd(rupdTag(),H4P_UI_TEXT,"s");
                            #endif
                            }
                        }
                        break;
                }
            }

            void        _updateFromUrl(bool fw,bool reboot){
                SYSINFO("START REMOTE UPDATE %s FROM %s v=%s\n",fw ? "Firmware":"FileSystem",CSTR(endpoint),H4P_VERSION);
                t_httpUpdate_return rv=fw ? update(_c,CSTR(endpoint),H4P_VERSION):updateSpiffs(_c,CSTR(endpoint),h4p.read("/h4UI").data());
                switch(rv){
                    case HTTP_UPDATE_OK:
                        if(reboot) QEVENT(H4PE_REBOOT);
                        break;
                    case HTTP_UPDATE_NO_UPDATES:
                        SYSINFO("Already up to date","");
                        break;
                    default:
                        SYSWARN("FAIL %d: %s", getLastError(), getLastErrorString().c_str());
                }
            }
    public:
        H4P_RemoteUpdate(const std::string& url=""): H4Service("rupd",H4PE_VIEWERS){
            h4p.gvSetstring(rupdTag(),url,true);
            _commonCTOR();
        }

                void        both(){ _entropise([this]{ _updateFromUrl(false,false); }); fw(); }
#if H4P_LOG_MESSAGES
                void        info() override { H4Service::info(); reply(" endpoint: %s",CSTR(endpoint)); }
#endif
                void        fs(){ _entropise([=]{ _updateFromUrl(false,true); }); }
                void        fw(){ _entropise([=]{ _updateFromUrl(true,true); }); }
//
                void        svcUp() override{ 
                    endpoint=std::string(h4p[rupdTag()]).append("/").append(h4p[deviceTag()]);
                    H4Service::svcUp();
                }
};