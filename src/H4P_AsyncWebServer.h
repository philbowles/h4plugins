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
#ifndef H4P_AsyncWebServer_HO
#define H4P_AsyncWebServer_HO

#include<H4PCommon.h>
#include<H4P_WiFiSelect.h>
#ifndef H4P_NO_WIFI
#include<H4P_WiFi.h>
#include<H4P_BinaryThing.h>
#include<ESPAsyncWebServer.h>

#define H4P_ASWS_EVT_TIMEOUT    30000

enum H4P_UI_TYPE {
    H4P_UI_LABEL,
    H4P_UI_TEXT,
    H4P_UI_BOOL,
    H4P_UI_ONOF
};

using H4_FN_UITXT      = function<string(void)>;
using H4_FN_UINUM      = function<int(void)>;
using H4_FN_UIBOOL     = function<bool(void)>;
using H4_FN_UIACTIVE   = function<void(string,string)>;

struct H4P_UI_ITEM {    
    string          id;
    H4P_UI_TYPE     type;
    H4_FN_UITXT     f;
    H4_FN_UIACTIVE  a;
};

using H4P_UI_LIST       = vector<H4P_UI_ITEM>;
class H4P_AsyncWebServer: public AsyncWebServer, public H4Plugin {
            uint32_t            _evtID=0;
            H4P_BinaryThing*    _btp=nullptr;
            AsyncEventSource*   _evts;
            H4_CMD_MAP          _local={};
            H4P_UI_LIST         _userItems={};
            vector<string>      _lines={};
            H4_FN_VOID          _onC,_onD;

            VSCMD(_msg);
            VSCMD(_uib);

                void         _rest(AsyncWebServerRequest *request);
//      service essentials
                void         _start() override;
                void         _stop() override;
                void         _hookIn() override;
                void         _greenLight() override {}; // do not autostart!
    public:
        H4P_AsyncWebServer(H4_FN_VOID onClientConnect=nullptr,H4_FN_VOID onClientDisconnect=nullptr): AsyncWebServer(80),H4Plugin(aswsTag()){
            _onC=onClientConnect;
            _onD=onClientDisconnect;

            _cmds={
                {_pName,    { H4PC_H4, _subCmd, nullptr}},
                {"msg",     { _subCmd, 0, CMDVS(_msg)}},
                {"uib",     { _subCmd, 0, CMDVS(_uib)}}
            };
        }
        static  String      aswsReplace(const String& var);
/*
                template<typename T>
                void uiAdd(const string& name,function<T(void)> f){
                    _uiAdd(name,H4P_UI_LABEL,[f]{ return stringFromInt(f()); });
                }
*/
                void        uiAddLabelText(const string& name,H4_FN_UITXT f){ _uiAdd(name,H4P_UI_LABEL,f); }
                void        uiAddLabelNumeric(const string& name,H4_FN_UINUM f){ _uiAdd(name,H4P_UI_LABEL,[f]{ return stringFromInt(f()); }); }
                void        uiAddBoolean(const string& name,H4_FN_UIBOOL f,H4_FN_UIACTIVE a=nullptr){ _uiAdd(name,H4P_UI_BOOL,[f]{ return stringFromInt(f()); },a); }
                void        uiAddGPIO(uint8_t pin);

                template<typename... Args>
                void        uiMessage(const string& msg, Args... args){ // variadic T<>
                    char* buff=static_cast<char*>(malloc(H4P_REPLY_BUFFER+1));
                    snprintf(buff,H4P_REPLY_BUFFER,CSTR(msg),args...);
                    _sendSSE(NULL,buff);
                    free(buff);
                }
                void        uiSetLabelNumeric(const string& name,const int f){ _sendSSE(CSTR(name),CSTR(stringFromInt(f))); }
                void        uiSetLabelText(const string& name,const string& value){ _sendSSE(CSTR(name),CSTR(value)); }
                void        uiSetBoolean(const string& name,const bool b){ uiSetLabelNumeric(name,b); }

                void        uiSync(const string& name);

//          syscall only
                void        _reply(string msg) override { _lines.push_back(msg); }
                void        _setBothNames(const string& host,const string& friendly);
                void        _sendSSE(const char* name,const char* msg);
                void        _uiAdd(const string& n,H4P_UI_TYPE t,H4_FN_UITXT f,H4_FN_UIACTIVE a=nullptr){ _userItems.push_back(H4P_UI_ITEM {n,t,f,a}); }
};

extern __attribute__((weak)) H4P_AsyncWebServer h4asws;

#endif
#endif // H4P_AsyncWebServer_H