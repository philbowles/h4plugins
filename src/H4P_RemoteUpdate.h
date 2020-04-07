/*
 MIT License

Copyright (c) 2019 Phil Bowles <H48266@gmail.com>
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
#ifdef ARDUINO_ARCH_ESP8266
#include<ESP8266httpUpdate.h>
#include<H4P_SerialCmd.h>
#define H4P_RUPD_STRETCH    5
/*
enum HTTPUpdateResult {
    HTTP_UPDATE_FAILED,
    HTTP_UPDATE_NO_UPDATES,
    HTTP_UPDATE_OK
};
*/
class H4P_RemoteUpdate: public H4Plugin {
                string      _url;

                void        _entropise(H4_FN_VOID f){ h4.onceRandom(H4P_PJ_LO,H4P_PJ_HI * H4P_RUPD_STRETCH,f); }
                void        _greenLight(){} // no autostart
                void        _hookIn(){ 
                    DEPEND(wifi);
/*                    
                    #ifdef H4P_LOG_EVENTS
                    ESPhttpUpdate.onProgress([this](int a,int b){ 
                        uint32_t pc=a*100/b;
                        static uint32_t prev;
                        if(!(pc%10)) {
                            if(pc > prev){
                                H4EVENT("%u",pc);
                                prev=pc; 
                            }
                        }
                    });
                    #endif                    ESPhttpUpdate.rebootOnUpdate(false);
*/
                }
                void        _updateFromUrl(bool fw,bool reboot){
                    string updateUrl=replaceAll((_url+"/"+_cb[boardTag()]+"/"+_cb["date"])," ","_");
                    h4wifi._downHooks();
                     t_httpUpdate_return rv=fw ? ESPhttpUpdate.update(CSTR(updateUrl),H4P_VERSION):ESPhttpUpdate.updateSpiffs(CSTR(updateUrl),CSTR(_cb["h4sv"]));
                    switch(rv){
                        case HTTP_UPDATE_OK:
                            if(reboot) h4reboot();
                            break;
                        case HTTP_UPDATE_NO_UPDATES:
                            h4wifi._upHooks();
                            H4EVENT("up2date");
                            break;
                        default:
                            h4wifi._upHooks();
                            H4EVENT("FAIL %d: %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                    }
                }
    public:
        H4P_RemoteUpdate(const string& url): _url(url),H4Plugin(rupdTag()){
            _cmds={
                {_pName,       { H4PC_H4,_subCmd  , nullptr}},
                {"both",       { _subCmd, 0, CMD(both)}},
                {"spiffs",     { _subCmd, 0, CMD(spiffs)}},
                {"update",     { _subCmd, 0, CMD(update)}}
            }; 
        }
        void both(){
            _entropise([this]{ _updateFromUrl(false,false); }); // spiffs, no reboot
            update();
        }
        void show() override { reply("url: %s",CSTR(_url)); }
        void spiffs(){ _entropise([this]{ _updateFromUrl(false,true); }); }
        void update(){ _entropise([this]{ _updateFromUrl(true,true); }); }
};
#endif
#endif // H4P_RemoteUpdate_H
