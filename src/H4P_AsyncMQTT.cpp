/*
 MIT License

Copyright (c) 2020 Phil Bowles <h4plugins@gmail.com>
   github     https://github.com/philbowles/esparto
   blog       https://8266iot.blogspot.com     
   groups     https://www.facebook.com/groups/esp8266questions/
              https://www.facebook.com/Esparto-Esp8266-Firmware-Support-2338535503093896/
                			  

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
#include<H4P_SerialCmd.h>
#include<H4P_AsyncMQTT.h>

// payload: scheme,broker,uname,pws,port e.g. https,192.168.1.4,,,1883
uint32_t H4P_AsyncMQTT::_change(std::vector<std::string> vs){  // broker,uname,pword,port
    return _guard1(vs,[this](std::vector<std::string> vs){
        auto vg=split(H4PAYLOAD,",");
        if(vg.size()!=5) return H4_CMD_PAYLOAD_FORMAT;
        std::string url=vg[0]+"://"+vg[1]+":"+vg[4];
        change(url,vg[2],vg[3]); // change this to a vs?
        return H4_CMD_OK;
    });
}

void H4P_AsyncMQTT::_handleEvent(const std::string& svc,H4PE_TYPE t,const std::string& msg){ 
    switch(t){
        case H4PE_VIEWERS:
            {
                uint32_t mode=STOI(msg);
                if(mode) {
                #if H4P_USE_WIFI_AP
                    if(WiFi.getMode()==WIFI_AP){
                        h4puiAdd(brokerTag(),H4P_UI_INPUT,"m");
                        h4puiAdd(mQuserTag(),H4P_UI_INPUT,"m");
                        h4puiAdd(mQpassTag(),H4P_UI_INPUT,"m");
                    }
                    else {
                        h4puiAdd(_me,H4P_UI_BOOL,"m","",H4P_UILED_BI);
                        h4puiAdd(brokerTag(),H4P_UI_TEXT,"m");
                        h4puiAdd(nDCXTag(),H4P_UI_TEXT,"m"); // cos we don't know it yet
                    }
                #else
                    h4puiAdd(_me,H4P_UI_BOOL,"m","",H4P_UILED_BI);
                    h4puiAdd(brokerTag(),H4P_UI_TEXT,"m");
                    h4puiAdd(nDCXTag(),H4P_UI_TEXT,"m"); // cos we don't know it yet
                #endif
                }
            }
            break;
        case H4PE_GVCHANGE:
//            if((svc==brokerTag()) && _running) {
            if(svc==brokerTag()) {
//                Serial.printf("RESTARTING DUE TO GVCHANGE\n");
                restart();
                break;
            }
            if(svc==stateTag()) {
                publishDevice(stateTag(),msg);
                break;
            }
    }
}

void H4P_AsyncMQTT::_init() {
    h4p.gvSetInt(_me,0,false);
    onMqttError([=](int e,int info){ XEVENT(H4PE_SYSWARN,"%d,%d",e,info); });

    std::string device=h4p[deviceTag()];
    if(_lwt.topic=="") {
        _lwt.topic=CSTR(std::string(prefix+device+"/offline"));
        _lwt.payload=CSTR(h4p[chipTag()]);
    }

    setWill(_lwt.topic,_lwt.QOS,_lwt.retain,_lwt.payload);
    prefix+=device+"/";

    onMqttMessage([=](const char* topic, const uint8_t* payload, size_t length, uint8_t qos, bool retain,bool dup){
        std::string top(topic);
        std::string pload((const char*) payload,length);
        h4.queueFunction([top,pload](){ h4p._executeCmd(CSTR(std::string(mqttTag()).append("/").append(top)),pload); },nullptr,H4P_TRID_MQMS);
    });

    onMqttConnect([=](bool b){
        h4.queueFunction([=](){
            _signalOff();
            h4.cancelSingleton(H4P_TRID_MQRC);
            _discoDone=false;
            subscribe(CSTR(std::string(allTag()).append(cmdhash())),0);
            subscribe(CSTR(std::string(device+cmdhash())),0);
            subscribe(CSTR(std::string(h4p[chipTag()]+cmdhash())),0);
            subscribe(CSTR(std::string(h4p[boardTag()]+cmdhash())),0);
            report();
            h4p[_me]=stringFromInt(_running=true);
            SYSINFO("CNX %s",CSTR(h4p[brokerTag()]));
            H4Service::svcUp();
        },nullptr,H4P_TRID_MQRC);
    });

    onMqttDisconnect([this](int8_t reason){
        if(!_discoDone){
            h4.queueFunction([this,reason](){
                _discoDone=true;
                h4p[_me]=stringFromInt(_running=false);
                h4p.gvInc(nDCXTag());
                SYSINFO("DCX %d",reason);
                H4Service::svcDown();
                if(autorestart && WiFi.status()==WL_CONNECTED) { h4.every(H4MQ_RETRY,[this](){
                    _signalBad(); // have to repeat to override hb if present: easiest to NIKE
                    connect(h4p[deviceTag()]); },nullptr,H4P_TRID_MQRC,true); 
                }// MUST be > 18sec due to shit lib ESpAsynTCP
            });
        }
    });
}

void H4P_AsyncMQTT::_setup(){ // allow for TLS
    if(h4p[brokerTag()]!="") setServer(CSTR(h4p[brokerTag()]),CSTR(h4p[mQuserTag()]),CSTR(h4p[mQpassTag()])); // optimise tag()
//    else SYSWARN("NO MQTT DETAILS","");
}

void H4P_AsyncMQTT::change(const std::string& broker,const std::string& user,const std::string& passwd){ // add creds
    XLOG("MQTT change to %s user=%s",CSTR(broker),CSTR(user));
    h4p[mQuserTag()]=user;
    h4p[mQpassTag()]=passwd;
    h4p[brokerTag()]=broker;
}

#if H4P_LOG_MESSAGES
void H4P_AsyncMQTT::info(){
    H4Service::info();
    reply(" Server: %s, %s",CSTR(h4p[brokerTag()]),connected() ? "CNX":"DCX");
    std::string reporting;
    for(auto const r:_reportList) reporting+=r+",";
    reporting.pop_back();
    reply(" Report: %s",CSTR(reporting));
}
#endif

void H4P_AsyncMQTT::report(){
    std::string j="{";
    for(auto const r:_reportList) {
        j+="\""+r+"\":\"";
        j.append(h4p[r]).append("\",");
    }
    j.pop_back();
    publishDevice(reportTag(),j+"}");
    XLOG("Reporting %s}",CSTR(j));
}

void H4P_AsyncMQTT::subscribeDevice(std::string topic,H4_FN_MSG f,H4PC_CMD_ID root){
    std::string fullTopic=std::string(h4p[deviceTag()])+"/"+topic; // refac
    if(topic.back()=='#' || topic.back()=='+'){
        topic.pop_back();
        topic.pop_back();
    }
    h4p.addCmd(topic,root,0,f);
    subscribe(CSTR(fullTopic),0);
    XLOG("Subscribed to %s",CSTR(fullTopic));
}

void H4P_AsyncMQTT::subscribeDevice(std::initializer_list<std::string> topic,H4_FN_MSG f,H4PC_CMD_ID root){ for(auto const& t:topic) subscribeDevice(t,f,root); }

void H4P_AsyncMQTT::svcUp(){
#if H4P_USE_WIFI_AP
    if(WiFi.getMode()==WIFI_AP) return;
#endif    
    _signalBad();
    _setup();
    autorestart=true;
    connect(h4p[deviceTag()]);
}

void H4P_AsyncMQTT::svcDown(){
    autorestart=false;
    disconnect();
}

void H4P_AsyncMQTT::unsubscribeDevice(std::string topic){
    std::string fullTopic=std::string(h4p[deviceTag()])+"/"+topic; // refac
    if(topic.back()=='#'){
        topic.pop_back();
        topic.pop_back();
    }
    h4p.removeCmd(topic);
    unsubscribe(CSTR(fullTopic));
    XLOG("Unsubscribed from %s\n",CSTR(topic));
}

void H4P_AsyncMQTT::unsubscribeDevice(std::initializer_list<std::string> topic){ for(auto const& t:topic) unsubscribeDevice(t); }