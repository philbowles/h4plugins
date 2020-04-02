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
#include<ESP8266httpUpdate.h>
//VARIANT
class H4P_RemoteUpdate: public H4Plugin {
                //bool        _reboot=false;
                string      _url;
                void        _greenLight(){} // no autostart
                void        _hookIn(){ DEPEND(wifi); }
                void        _updateFromUrl(bool fw,bool reboot){
                    t_httpUpdate_return ret;
                    string updateUrl=_url+"/"+_cb[boardTag()];
                    ESPhttpUpdate.rebootOnUpdate(false);
                    ret=fw ? ESPhttpUpdate.update(CSTR(updateUrl),H4P_VERSION):ESPhttpUpdate.updateSpiffs(CSTR(updateUrl),H4P_VERSION);
                    H4EVENT("FH=%d _updateFromUrl reply %d",ESP.getFreeHeap(),ret);
                    if(ret==HTTP_UPDATE_OK && reboot) h4reboot();
                }
    public:
        H4P_RemoteUpdate(const string& url,bool runAtStart=false): _url(url),H4Plugin(rupdTag()){
            _cmds={
                {_pName,       { H4PC_H4,_subCmd  , nullptr}},
                {"both",       { _subCmd, 0, CMD(both)}},
                {"spiffs",     { _subCmd, 0, CMD(spiffs)}},
                {"update",     { _subCmd, 0, CMD(update)}}
            }; 
            if(runAtStart) h4.onceRandom(H4P_PJ_LO,H4P_PJ_HI,[this](){ both(); });
        }
        void both(){
            _updateFromUrl(false,false); // spiffs, no reboot
            _updateFromUrl(true,true);
        }
        void show() override { reply("url: %s\n",CSTR(_url)); }
        void spiffs(){ _updateFromUrl(false,true); }
        void update(){ _updateFromUrl(true,true); }
};

#endif // H4P_RemoteUpdate_H
