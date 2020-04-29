/*
 MIT License

Copyright (c) 2019 Phil Bowles <h4plugins@gmail.com>
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
#ifndef H4P_NO_WIFI

uint32_t H4P_AsyncMQTT::_change(vector<string> vs){
    return guardString2(vs,[this](string a,string b){ 
        if(isNumeric(b)) change(a,atoi(CSTR(b)));
    });
}

void H4P_AsyncMQTT::_greenLight(){
    _setup();
    onMessage([this](char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t length, size_t index, size_t total){
        h4.queueFunction(
            bind([](string topic, string pload){ 
                h4cmd._executeCmd(CSTR(string(mqttTag()).append("/").append(topic)),pload); 
            },string(topic),stringFromBuff((byte*) payload,length)),
        nullptr,H4P_TRID_MQMS);
    });	
    
    onConnect([this](bool b){
        h4.queueFunction([this](){
            h4.cancelSingleton(H4P_TRID_MQRC);
            _discoDone=false;
            subscribe(CSTR(string("all").append(cmdhash())),0);
            subscribe(CSTR(string(device+cmdhash())),0);
            subscribe(CSTR(string(_cb[chipTag()]+cmdhash())),0);
            subscribe(CSTR(string(_cb[boardTag()]+cmdhash())),0);
            publish("h4/online",0,false,CSTR(device));
            _upHooks();
            H4EVENT("MQTT CNX");
        });
    });
/*
  TCP_DISCONNECTED = 0,

  MQTT_UNACCEPTABLE_PROTOCOL_VERSION = 1,
  MQTT_IDENTIFIER_REJECTED = 2,
  MQTT_SERVER_UNAVAILABLE = 3,
  MQTT_MALFORMED_CREDENTIALS = 4,
  MQTT_NOT_AUTHORIZED = 5,

  ESP8266_NOT_ENOUGH_SPACE = 6,

  TLS_BAD_FINGERPRINT = 7
*/
    onDisconnect([this](AsyncMqttClientDisconnectReason reason){
        if(!_discoDone){
            h4.queueFunction([this,reason](){
                _discoDone=true;
                _downHooks();
                H4EVENT("MQTT DCX %d",reason);
                if(autorestart && WiFi.status()==WL_CONNECTED) h4.every(H4MQ_RETRY,[this](){ connect(); },nullptr,H4P_TRID_MQRC,true);
            });
        }
    });
}

void H4P_AsyncMQTT::_hookIn() { DEPEND(wifi); }

void H4P_AsyncMQTT::_setup(){
    device=_cb[deviceTag()];
    setClientId(CSTR(device));
    if(_lwt.topic=="") {
        _lwt.topic="h4/offline";
        _lwt.payload=CSTR(device);
    }
//    Serial.printf("USING LWT %s [%s] qos=%d R=%d\n",CSTR(_lwt.topic),CSTR(_lwt.payload),_lwt.QOS,_lwt.retain);
//    setWill(CSTR(_lwt.topic),_lwt.QOS,_lwt.retain,CSTR(_lwt.payload));
    Serial.printf("USING LWT %s [%s] qos=%d R=%d\n",_lwt.topic,_lwt.payload,_lwt.QOS,_lwt.retain);
    setWill(_lwt.topic,_lwt.QOS,_lwt.retain,_lwt.payload);

    string broker=_cb[brokerTag()];
    uint16_t port=atoi(CSTR(_cb[portTag()]));
    if(atoi(CSTR(broker))) {
        vector<string> vs=split(broker,".");
        setServer(IPAddress(PARAM_INT(0),PARAM_INT(1),PARAM_INT(2),PARAM_INT(3)),port);
    } else setServer(CSTR(broker),port);
        
    if(_cb["muser"]!="") setCredentials(CSTR(_cb["muser"]),CSTR(_cb["mpasswd"]));
}

void H4P_AsyncMQTT::_start(){ 
//    if(!(WiFi.getMode() & WIFI_AP)) {
        autorestart=true;
        connect(); 
//    }
}

void H4P_AsyncMQTT::_stop(){
//    if(!(WiFi.getMode() & WIFI_AP)) {
        autorestart=false;
        disconnect(true);
//    }
}

void H4P_AsyncMQTT::change(const string& broker,uint16_t port){ // add creds
    stop();
    _cb[brokerTag()]=broker;
    _cb[portTag()]=stringFromInt(port);
    _setup();
    start();
}

void H4P_AsyncMQTT::publishDevice(const string& topic,const string& payload){ 
    publish(CSTR(string("h4/"+device+"/"+topic)),0,false,CSTR(payload));
}

void H4P_AsyncMQTT::subscribeDevice(string topic,H4_FN_MSG f,H4PC_CMD_ID root){
    string fullTopic=device+"/"+topic;
    if(topic.back()=='#'){
        topic.pop_back();
        topic.pop_back();
    }
    h4cmd.addCmd(topic,root,0,f);
    subscribe(CSTR(fullTopic),0);
    H4EVENT("Subscribed to %s",CSTR(topic));
}
void H4P_AsyncMQTT::unsubscribeDevice(string topic){
    string fullTopic=device+"/"+topic; // refactor
    if(topic.back()=='#'){
        topic.pop_back();
        topic.pop_back();
    }
    h4cmd.removeCmd(topic);
    unsubscribe(CSTR(fullTopic));
    H4EVENT("Unsubscribed from %s\n",CSTR(topic));
}
#endif