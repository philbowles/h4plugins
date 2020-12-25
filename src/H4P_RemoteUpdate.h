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
                WiFiClient  _c;
                string      _url;
                void        _entropise(H4_FN_VOID f){ h4.onceRandom(H4P_PJ_LO,H4P_PJ_HI * H4P_RUPD_STRETCH,f); }
                void        _greenLight(){} // no autostart
                void        _hookIn(){ DEPEND(wifi); }
                void        _updateFromUrl(bool fw,bool reboot){
                    String updateUrl=CSTR(replaceAll((_url+"/"+_cb[boardTag()]+"/"+_cb[h4pvTag()])," ","_"));
                    String version=fw ? CSTR(_cb[h4pvTag()]):CSTR(_cb[h4svTag()]); // tag it
                    H4EVENT("%s [%s]",CSTR(updateUrl),CSTR(version));
                    h4wifi._downHooks();
                    t_httpUpdate_return rv=fw ? update(_c,updateUrl,version):updateSpiffs(_c,updateUrl,version);
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
                            H4EVENT("FAIL %d: %s", getLastError(), getLastErrorString().c_str());
                    }
                }
    public:
        H4P_RemoteUpdate(const string& url): _url(url),H4Plugin(rupdTag()){
            _cmds={
                {_pName,       { H4PC_H4,_subCmd  , nullptr}},
                {"both",       { _subCmd, 0, CMD(both)}},
                {"fs",         { _subCmd, 0, CMD(spiffs)}},
                {"update",     { _subCmd, 0, CMD(firmware)}}
            }; 
        }
        void both(){
            _entropise([this]{ _updateFromUrl(false,false); }); // spiffs, no reboot
            firmware();
        }
        void show() override { reply("url: %s",CSTR(_url)); }
        void spiffs(){ _entropise([this]{ _updateFromUrl(false,true); }); }
        void firmware(){ _entropise([this]{ _updateFromUrl(true,true); }); }
};

#endif // H4P_RemoteUpdate_H
