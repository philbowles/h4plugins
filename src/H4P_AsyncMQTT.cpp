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
#include<H4P_AsyncMQTT.h>
#include<H4P_WiFi.h>

void __attribute__((weak)) onMQTTError(uint8_t e,int info){ SEVENT(H4P_EVENT_MQTT_ERROR,"e=%d info=%d",e,info); }

uint32_t H4P_AsyncMQTT::_change(vector<string> vs){
    return _guard1(vs,[this](vector<string> vs){
        auto vg=split(H4PAYLOAD,",");
        if(vg.size()!=4) return H4_CMD_PAYLOAD_FORMAT;
        if(!stringIsNumeric(vg[1])) return H4_CMD_NOT_NUMERIC;
        change(vg[0],STOI(vg[1]),vg[2],vg[3]); // change this to a vs?
        return H4_CMD_OK;
    });
}

void H4P_AsyncMQTT::_handleEvent(H4PID pid,H4P_EVENT_TYPE t,const string& msg){ _pWiFi->_wipe(mqconfTag()); }

void H4P_AsyncMQTT::_hookIn() { 
    _pWiFi=h4pdepend<H4P_WiFi>(this,H4PID_WIFI);
#if H4P_USE_WIFI_AP
    if(_pWiFi->_getPersistentValue(mqconfTag(),"")){
        vector<string> mqconf=split(_cb[mqconfTag()],",");
        _cb[brokerTag()]=mqconf[0];
        _cb[portTag()]=mqconf[1];
        _cb[mQuserTag()]=mqconf.size() > 2 ? mqconf[2]:"";
        _cb[mQpassTag()]=mqconf.size() > 3 ? mqconf[3]:"";
    }
    _cb.erase(mqconfTag());
#endif
    onError(onMQTTError);

    device=_cb[deviceTag()];
    setClientId(CSTR(device));
    if(_lwt.topic=="") {
        _lwt.topic=CSTR(string(prefix+device+"/offline"));
        _lwt.payload=CSTR(_cb[chipTag()]);
    }
    setWill(_lwt.topic,_lwt.QOS,_lwt.retain,_lwt.payload);
    prefix+=device+"/";

    onMessage([this](const char* topic, const uint8_t* payload, size_t length, uint8_t qos, bool retain,bool dup){
        string top(topic);
        string pload=stringFromBuff((uint8_t*) payload,length);
        h4.queueFunction([top,pload](){ h4cmd._executeCmd(CSTR(string(mqttTag()).append("/").append(top)),pload); },nullptr,H4P_TRID_MQMS);
    });

    onConnect([this](bool b){
        h4.queueFunction([this](){
            _pWiFi->signalOff();
            h4.cancelSingleton(H4P_TRID_MQRC);
            _discoDone=false;
            subscribe(CSTR(string("all").append(cmdhash())),0);
            subscribe(CSTR(string(device+cmdhash())),0);
            subscribe(CSTR(string(_cb[chipTag()]+cmdhash())),0);
            subscribe(CSTR(string(_cb[boardTag()]+cmdhash())),0);
            report();
            _upHooks();
            _pWiFi->uiSync();
            PLOG("MQTT CNX");
        });
    });

    onDisconnect([this](int8_t reason){
        if(!_discoDone){
            h4.queueFunction([this,reason](){
                _badSignal();
                _discoDone=true;
                _downHooks();
                _pWiFi->uiSync();
                PLOG("MQTT DCX %d",reason);
                if(autorestart && WiFi.status()==WL_CONNECTED) h4.every(H4MQ_RETRY,[this](){ connect(); },nullptr,H4P_TRID_MQRC,true);
            });
        }
    });
    H4Plugin::_hookIn();
}

void H4P_AsyncMQTT::_greenLight(){
    if(WiFi.getMode()==WIFI_STA){
        _pWiFi->_uiAdd(H4P_UIO_PMV,"Pangolin",H4P_UI_LABEL,_cb[pmvTag()]);
        _pWiFi->_uiAdd(H4P_UIO_MQB,brokerTag(),H4P_UI_LABEL,"",[]{ return _cb[brokerTag()]; }); // cos we don't know it yet
        _pWiFi->_uiAdd(H4P_UIO_MQP,portTag(),H4P_UI_LABEL,"",[]{ return _cb[portTag()]; }); // cos we don't know it yet
        _pWiFi->_uiAdd(H4P_UIO_MQTT,uppercase(mqttTag()),H4P_UI_BOOL,"",
        [this]{ return stringFromInt(_state()); },
        [this](const string& b){ 
            if(b=="1") _start();
            else _stop();
            PLOG("MQTT %s by user",b=="1" ? "Started":"Stopped");
            _pWiFi->uiMessage("MQTT %s by user",b=="1" ? "Started":"Stopped");
        });
    }
}

void H4P_AsyncMQTT::_setup(){ // allow for TLS
    string broker=_cb[brokerTag()];
    uint16_t port=atoi(CSTR(_cb[portTag()]));
    setServer(CSTR(broker),port);
    if(_cb[mQuserTag()]!="") setCredentials(CSTR(_cb[mQuserTag()]),CSTR(_cb[mQpassTag()])); // optimise tag()
}

void H4P_AsyncMQTT::_start(){
    if(WiFi.getMode()==WIFI_STA) {
        _badSignal();
        _setup();
        autorestart=true;
        connect();
    }
}

void H4P_AsyncMQTT::_stop(){
    autorestart=false;
    disconnect(true);
}

void H4P_AsyncMQTT::change(const string& broker,uint16_t port,const string& user,const string& passwd){ // add creds
    PLOG("MQTT change to %s:%d user=%s",CSTR(broker),port,CSTR(user));
    H4Plugin::stop();
    _cb[brokerTag()]=broker;
    _cb[portTag()]=stringFromInt(port);
    _cb[mQuserTag()]=user;
    _cb[mQpassTag()]=passwd;
#if H4P_USE_WIFI_AP
    _cb[mqconfTag()]=_cb[brokerTag()]+","+_cb[portTag()]+","+_cb[mQuserTag()]+","+_cb[mQpassTag()];
    _pWiFi->_save(mqconfTag());
#endif
    restart();
}

void H4P_AsyncMQTT::report(){
    string j="{";
    for(auto const r:_reportList) j+="\""+r+"\":\""+_cb[r]+"\",";
    j.pop_back();
    publishDevice("report",j+"}");
    PLOG("Reporting %s}",CSTR(j));
}

void H4P_AsyncMQTT::show(){
    reply("Server: %s, port:%s, %s",CSTR(_cb[brokerTag()]),CSTR(_cb[portTag()]),connected() ? "CNX":"DCX");
    string reporting;
    for(auto const r:_reportList) reporting+=r+",";
    reporting.pop_back();
    reply("Report: %s",CSTR(reporting));
}

void H4P_AsyncMQTT::subscribeDevice(string topic,H4_FN_MSG f,H4PC_CMD_ID root){
    string fullTopic=device+"/"+topic;
    if(topic.back()=='#' || topic.back()=='+'){
        topic.pop_back();
        topic.pop_back();
    }
    h4cmd.addCmd(topic,root,0,f);
    subscribe(CSTR(fullTopic),0);
    PLOG("Subscribed to %s",CSTR(fullTopic));
}

void H4P_AsyncMQTT::unsubscribeDevice(string topic){
    string fullTopic=device+"/"+topic; // refactor
    if(topic.back()=='#'){
        topic.pop_back();
        topic.pop_back();
    }
    h4cmd.removeCmd(topic);
    unsubscribe(CSTR(fullTopic));
    PLOG("Unsubscribed from %s\n",CSTR(topic));
}