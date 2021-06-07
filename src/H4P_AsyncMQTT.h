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

#include<H4Service.h>
#include<H4P_Signaller.h>
#include<H4P_WiFi.h>
#include<PangolinMQTT.h>

struct H4P_LWT {
    const char*     topic;
    const char*     payload;
    int             QOS;
    bool            retain;
};

STAG(nDCX);

class H4P_AsyncMQTT: public H4Service, public PangolinMQTT{
                bool            _discoDone;
                bool            autorestart=true;
                std::string     prefix=std::string(h4Tag()).append("/");
                struct H4P_LWT  _lwt;
                std::unordered_set<std::string> _reportList={binTag(),ipTag(),h4pTag()};

                VSCMD(_change);

                void        _commonStartup(){
                    h4p.gvSetInt(nDCXTag(),0,false);
                    depend<H4P_WiFi>(wifiTag());
                    _addLocals({
                        {_me,          { H4PC_H4, _pid, nullptr }}, 
                        {changeTag(),  { _pid,       0, CMDVS(_change) }},
                        {reportTag(),  { _pid,       0, CMD(report) }}
                    });
                }
                void        _setup();
                void        _signalBad(){ H4P_Signaller::signal(H4P_SIG_MORSE,"..   ,150"); }
                void        _signalOff(){ H4P_Signaller::signal(H4P_SIG_STOP); }
    protected:
        virtual void        _handleEvent(const std::string& svc,H4PE_TYPE t,const std::string& msg) override;
    public:
#if H4P_USE_WIFI_AP
        H4P_AsyncMQTT(H4P_LWT lwt={"","",0,false}): _lwt(lwt), H4Service(mqttTag(),H4PE_GVCHANGE|H4PE_VIEWERS){
            h4p.gvSetstring(brokerTag(),"",true);
            h4p.gvSetstring(mQuserTag(),"",true);
            h4p.gvSetstring(mQpassTag(),"",true);
            _commonStartup();
        }
#else
        explicit H4P_AsyncMQTT(): H4Service(mqttTag(),H4PE_NOOP){}
        H4P_AsyncMQTT(std::string url, std::string user="",std::string pass="",H4P_LWT lwt={"","",0,false}): _lwt(lwt), H4Service(mqttTag(),H4PE_GVCHANGE|H4PE_VIEWERS){
            h4p.gvSetstring(brokerTag(),url,true);
            h4p.gvSetstring(mQuserTag(),user,true);
            h4p.gvSetstring(mQpassTag(),pass,true);
            _commonStartup();
        }
#endif
                void        addReportingItem(const std::string& ri){ _reportList.insert(ri); }
                void        change(const std::string& broker,const std::string& user,const std::string& passwd);
                void        publishDevice(const std::string& topic,const std::string& payload,uint8_t qos=0, bool retain=false){ xPublish(CSTR(std::string(prefix+topic)),payload,qos,retain); }
                void        publishDevice(const std::string& topic,uint32_t payload,uint8_t qos=0, bool retain=false){ publishDevice(topic,stringFromInt(payload),qos,retain); }
                void        report();
                void        restart() { autorestart=true; svcDown(); svcUp(); }
#if H4P_LOG_MESSAGES
                void        info() override;
#endif
                void        subscribeDevice(std::string topic,H4_FN_MSG f,H4PC_CMD_ID root=H4PC_ROOT);
                void        unsubscribeDevice(std::string topic);
                void        subscribeDevice(std::initializer_list<std::string> topic,H4_FN_MSG f,H4PC_CMD_ID root=H4PC_ROOT);
                void        unsubscribeDevice(std::initializer_list<std::string> topic);
//
        virtual void        svcUp() override;
        virtual void        svcDown() override;
//          syscall only
        virtual void        _init() override;
                void        _reply(std::string msg) override { publishDevice("reply",msg); }
};