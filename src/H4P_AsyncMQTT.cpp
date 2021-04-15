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

uint32_t H4P_AsyncMQTT::_change(vector<string> vs){  // broker,uname,pword,port
    return _guard1(vs,[this](vector<string> vs){
        auto vg=split(H4PAYLOAD,",");
        if(vg.size()!=4) return H4_CMD_PAYLOAD_FORMAT;
        if(!stringIsNumeric(vg[3])) return H4_CMD_NOT_NUMERIC;
        change(vg[0],vg[1],vg[2],STOI(vg[3])); // change this to a vs?
        return H4_CMD_OK;
    });
}

void H4P_AsyncMQTT::_handleEvent(const string& svc,H4PE_TYPE t,const string& msg){ 
    switch(t){
        case H4PE_VIEWERS:
            {
                uint32_t mode=STOI(msg);
                if(mode) {
                #if H4P_USE_WIFI_AP
                    if(WiFi.getMode()==WIFI_AP){
                        h4puiAdd(brokerTag(),H4P_UI_INPUT,"m");
                        h4puiAdd(portTag(),H4P_UI_INPUT,"m");
                        h4puiAdd(mQuserTag(),H4P_UI_INPUT,"m");
                        h4puiAdd(mQpassTag(),H4P_UI_INPUT,"m");
                    }
                    else {
                        h4puiAdd("Pangolin",H4P_UI_TEXT,"m",h4p[pmvTag()]); // mhang!
                        h4puiAdd(_me,H4P_UI_BOOL,"m","",H4P_UILED_BI);
                        h4puiAdd(brokerTag(),H4P_UI_TEXT,"m");
                        h4puiAdd(portTag(),H4P_UI_TEXT,"m");
                        h4puiAdd(nDCXTag(),H4P_UI_TEXT,"m"); // cos we don't know it yet
                    }
                #else
                    h4puiAdd("Pangolin",H4P_UI_TEXT,"m",h4p[pmvTag()]); // mhang!
                    h4puiAdd(_me,H4P_UI_BOOL,"m","",H4P_UILED_BI);
                    h4puiAdd(brokerTag(),H4P_UI_TEXT,"m");
                    h4puiAdd(portTag(),H4P_UI_TEXT,"m");
                    h4puiAdd(nDCXTag(),H4P_UI_TEXT,"m"); // cos we don't know it yet
                #endif
                }
            }
            break;
        case H4PE_GVCHANGE:
            if((svc==brokerTag() || svc==portTag()) && _running) {
                Serial.printf("RESTARTING DUE TO GVCHANGE\n");
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
/*
    Serial.printf("H4P_AsyncMQTT::_init()\n");
    Serial.printf("Broker %s\n",CSTR(h4p[brokerTag()]));
    Serial.printf("Port %s\n",CSTR(h4p[portTag()]));
    Serial.printf("MQU %s\n",CSTR(h4p[mQuserTag()]));
    Serial.printf("MQP %s\n",CSTR(h4p[mQpassTag()]));
*/
    onError([=](uint8_t && e,int && info){ XEVENT(H4PE_SYSWARN,"%d,%d",e,info); });

    device=h4p[deviceTag()];
    setClientId(CSTR(device));
    if(_lwt.topic=="") {
        _lwt.topic=CSTR(string(prefix+device+"/offline"));
        _lwt.payload=CSTR(h4p[chipTag()]);
    }
    setWill(_lwt.topic,_lwt.QOS,_lwt.retain,_lwt.payload);
    prefix+=device+"/";

    onMessage([this](const char* topic, const uint8_t* payload, size_t length, uint8_t qos, bool retain,bool dup){
        string top(topic);
        string pload=stringFromBuff((uint8_t*) payload,length);
        h4.queueFunction([top,pload](){ h4p._executeCmd(CSTR(string(mqttTag()).append("/").append(top)),pload); },nullptr,H4P_TRID_MQMS);
    });

    onConnect([this](bool b){
        h4.queueFunction([this](){
            _signalOff();
            h4.cancelSingleton(H4P_TRID_MQRC);
            _discoDone=false;
            subscribe(CSTR(string(allTag()).append(cmdhash())),0);
            subscribe(CSTR(string(device+cmdhash())),0);
            subscribe(CSTR(string(h4p[chipTag()]+cmdhash())),0);
            subscribe(CSTR(string(h4p[boardTag()]+cmdhash())),0);
            report();
            h4p[_me]=stringFromInt(_running=true);
            SYSINFO("CNX %s:%s",CSTR(h4p[brokerTag()]),CSTR(h4p[portTag()]));
            H4Service::svcUp();
        });
    });

    onDisconnect([this](int8_t reason){
        if(!_discoDone){
            h4.queueFunction([this,reason](){
                _signalBad();
                _discoDone=true;
                h4p[_me]=stringFromInt(_running=false);
                h4p.gvInc(nDCXTag());
                SYSINFO("DCX %d",reason);
                H4Service::svcDown();
                if(autorestart && WiFi.status()==WL_CONNECTED) { h4.every(H4MQ_RETRY,[this](){ connect(); },nullptr,H4P_TRID_MQRC,true); }// MUST be > 18sec due to shit lib ESpAsynTCP
            });
        }
    });
}

void H4P_AsyncMQTT::_setup(){ // allow for TLS
    setServer(CSTR(h4p[brokerTag()]),h4p.gvGetInt(portTag()));
    if(h4p[mQuserTag()]!="") setCredentials(CSTR(h4p[mQuserTag()]),CSTR(h4p[mQpassTag()])); // optimise tag()
}
/*
void H4P_AsyncMQTT::_update(const string& name,const string& value){
    h4p[name]=value;
    h4puiSync(name,value);
}
*/
void H4P_AsyncMQTT::change(const string& broker,const string& user,const string& passwd,uint16_t port){ // add creds
    XLOG("MQTT change to %s:%d user=%s",CSTR(broker),port,CSTR(user));
    h4p[mQuserTag()]=user;
    h4p[mQpassTag()]=passwd;
    h4p[portTag()]=stringFromInt(port);
    h4p[brokerTag()]=broker;
}

#if H4P_LOG_MESSAGES
void H4P_AsyncMQTT::info(){
    H4Service::info();
    reply(" Server: %s, port:%s, %s",CSTR(h4p[brokerTag()]),CSTR(h4p[portTag()]),connected() ? "CNX":"DCX");
    string reporting;
    for(auto const r:_reportList) reporting+=r+",";
    reporting.pop_back();
    reply(" Report: %s",CSTR(reporting));
}
#endif

void H4P_AsyncMQTT::report(){
    string j="{";
    for(auto const r:_reportList) {
        j+="\""+r+"\":\"";
        j.append(h4p[r]).append("\",");
    }
    j.pop_back();
    publishDevice(reportTag(),j+"}");
    XLOG("Reporting %s}",CSTR(j));
}

void H4P_AsyncMQTT::subscribeDevice(string topic,H4_FN_MSG f,H4PC_CMD_ID root){
    string fullTopic=device+"/"+topic;
    if(topic.back()=='#' || topic.back()=='+'){
        topic.pop_back();
        topic.pop_back();
    }
    h4p.addCmd(topic,root,0,f);
    subscribe(CSTR(fullTopic),0);
    XLOG("Subscribed to %s",CSTR(fullTopic));
}

void H4P_AsyncMQTT::svcUp(){
#if H4P_USE_WIFI_AP
    if(WiFi.getMode()==WIFI_AP) return;
#endif    
    _signalBad();
    _setup();
    autorestart=true;
    connect();
}

void H4P_AsyncMQTT::svcDown(){
    autorestart=false;
    disconnect(true);
}

void H4P_AsyncMQTT::unsubscribeDevice(string topic){
    string fullTopic=device+"/"+topic; // refactor
    if(topic.back()=='#'){
        topic.pop_back();
        topic.pop_back();
    }
    h4p.removeCmd(topic);
    unsubscribe(CSTR(fullTopic));
    XLOG("Unsubscribed from %s\n",CSTR(topic));
}