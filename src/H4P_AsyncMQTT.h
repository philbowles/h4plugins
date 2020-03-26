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
copies of the Software, and to permit persons to whom the Software iss
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
#ifndef H4P_AsyncMQTT_HO
#define H4P_AsyncMQTT_HO

#include<H4PCommon.h>
#include<H4P_SerialCmd.h>
#include<H4P_WiFiSelect.h>
#ifndef H4P_NO_WIFI
#include<H4P_WiFi.h>

#include<AsyncMqttClient.h>

class H4P_AsyncMQTT: public H4Plugin, public AsyncMqttClient{
//        unordered_set<string>   _grid;
            bool            autorestart=true;
            string          device;

        VSCMD(_change);
        VSCMD(_offline);
        VSCMD(_online);

                void        _setup();
                void        _start() override;
                bool        _state() override { return connected(); }
                void        _stop() override;
                void        _hookIn() override;
                void        _greenLight() override {} // do not autostart!
    public:
        H4P_AsyncMQTT(string broker,uint16_t port, string user="",string pass="",H4_FN_VOID onC=nullptr,H4_FN_VOID onD=nullptr):
            H4Plugin(mqttTag(),onC,onD)
        {
            _cb["broker"]=broker;
            _cb[portTag()]=stringFromInt(port);
            _cb["muser"]=user,
            _cb["mpasswd"]=pass;

            _cmds={
                {_pName,    { H4PC_ROOT, _subCmd, nullptr}},
                {"change",  { _subCmd, 0, CMDVS(_change) }}
//                {"grid",    { _subCmd, 0, CMD(showGrid) }},
//                {"offline", { _subCmd, 0, CMDVS(_offline) }},
//                {"online",  { _subCmd, 0, CMDVS(_online) }}
            };       
        }
                void        change(const string& broker,uint16_t port);
                void        publishDevice(const string& topic,const string& payload="");
                void        publishDevice(const string& topic,uint32_t payload){ publishDevice(topic,stringFromInt(payload)); }
                void        subscribeDevice(string topic,H4_FN_MSG f);
                void        unsubscribeDevice(string topic);

    //          syscall only
                void        _reply(string msg) override { publish(CSTR(string("h4/"+device+"/reply")),0,false,CSTR(msg)); }
};

    extern __attribute__((weak)) H4P_AsyncMQTT h4mqtt;
#endif

#endif // H4P_AsyncMQTT_H