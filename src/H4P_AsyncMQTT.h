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
#pragma once

#include<H4PCommon.h>
#include<H4P_SerialCmd.h>
#include<H4P_WiFi.h>
#include<PangolinMQTT.h>

struct H4P_LWT {
    const char*     topic;
    const char*     payload;
    int             QOS;
    bool            retain;
};

class H4P_AsyncMQTT: public H4Plugin, public PangolinMQTT{
                H4P_WiFi*       _pWiFi;
                bool            autorestart=true;
                string          device;
                string          prefix=string(h4Tag()).append("/");
                struct H4P_LWT  _lwt;
                unordered_set<string>  _reportList={"bin",boardTag(),ipTag(),h4pTag(),h4UITag(),pmvTag()};

                VSCMD(_change);

                void        _badSignal(){ h4pisloaded<H4P_WiFi>(H4PID_WIFI)->signal(". .    ",H4P_SIGNAL_TIMEBASE/2); }
                void        _greenLight() override; //do not autostart!
        virtual void        _handleEvent(H4PID pid,H4P_EVENT_TYPE t,const string& msg) override;
                void        _setup();
                void        _start() override;
                bool        _state() override { return connected(); }
                void        _stop() override;
    public:
#if H4P_USE_WIFI_AP
        H4P_AsyncMQTT(H4_FN_VOID onC=nullptr,H4_FN_VOID onD=nullptr,H4P_LWT lwt={"","",0,false}):
            _lwt(lwt), H4Plugin(H4PID_MQTT,H4P_EVENT_FACTORY,onC,onD)
        {
#else
        explicit H4P_AsyncMQTT():H4Plugin(H4PID_MQTT,H4P_EVENT_FACTORY){}
        H4P_AsyncMQTT(string broker,uint16_t port, string user="",string pass="",H4_FN_VOID onC=nullptr,H4_FN_VOID onD=nullptr,H4P_LWT lwt={"","",0,false}):
            _lwt(lwt), H4Plugin(H4PID_MQTT,H4P_EVENT_FACTORY,onC,onD)
        {
            _cb[brokerTag()]=broker;
            _cb[portTag()]=stringFromInt(port);
            _cb[mQuserTag()]=user,
            _cb[mQpassTag()]=pass;
#endif
            _cb[pmvTag()]=PANGO_VERSION;
            _addLocals({
                {_pName,    { H4PC_H4, _pid, nullptr }}, 
                {"change",  { _pid, 0, CMDVS(_change) }},
                {"report",  { _pid, 0, CMD(report) }}
            });
        }
                void        addReportingItem(const string& ri){ _reportList.insert(ri); }
                void        change(const string& broker,uint16_t port,const string& user,const string& passwd);
                void        publishDevice(const string& topic,const string& payload,uint8_t qos=0, bool retain=false){ xPublish(CSTR(string(prefix+topic)),payload,qos,retain); }
                void        publishDevice(const string& topic,uint32_t payload,uint8_t qos=0, bool retain=false){ publishDevice(topic,stringFromInt(payload),qos,retain); }
                void        report();
                void        show() override;
                void        subscribeDevice(string topic,H4_FN_MSG f,H4PC_CMD_ID root=H4PC_ROOT);
                void        unsubscribeDevice(string topic);
    //          syscall only
                void        _hookIn() override;
                void        _reply(string msg) override { publishDevice("reply",msg); }
};