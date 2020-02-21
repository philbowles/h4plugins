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
#ifndef H4P_AsyncWebServer_HO
#define H4P_AsyncWebServer_HO

#include<H4PCommon.h>
#include<H4P_WiFiSelect.h>
#include<H4P_WiFi.h>
#ifndef H4P_NO_WIFI
#include<H4P_CmdErrors.h>
#include<H4P_SerialCmd.h>
    #include<ESPAsyncWebServer.h>

class H4P_AsyncWebServer: public AsyncWebServer, public H4PluginService {
            H4_CMD_MAP          _local={};
//
            vector<string>      lines={};

                void         _hookIn();
                void         _rest(AsyncWebServerRequest *request);
                bool         _webAuth(AsyncWebServerRequest *request);

    public:
        H4P_AsyncWebServer(string admin="admin",string passwd="admin",H4_FN_VOID onC=[](){},H4_FN_VOID onD=[](){}):
            AsyncWebServer(80),
            H4PluginService(onC,onD)
        {
            _cb["auser"]=admin;
            _cb["apasswd"]=passwd;
            _pid=aswsTag();
            _names={{H4P_TRID_ASWS,uppercase(_pid)}};
            _cmds={};
        }
//      service essentials
                void         start() override;
                void         stop() override;
//
        static String       aswsReplace(const String& var);
//          syscall only
                void        _reply(string msg) override { lines.push_back(msg); };
};

extern __attribute__((weak)) H4P_AsyncWebServer h4asws;

#endif
#endif // H4P_AsyncWebServer_H