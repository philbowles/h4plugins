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
        if(isNumeric(b)){
            change(a,atoi(CSTR(b))); 
        }
    });
}

void H4P_AsyncMQTT::_hookIn() {
    if(H4Plugin::isLoaded(wifiTag()) ){
        _setup();

        onMessage([this](char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t length, size_t index, size_t total){
            h4.queueFunction(
                bind([](string topic, string pload){ 
                    h4sc._executeCmd(CSTR(string(mqttTag()).append("/").append(topic)),pload); 
                },string(topic),stringFromBuff((byte*) payload,length)),
            nullptr,H4P_TRID_MQMS);

        });	
        
        onConnect([this](bool b){
            h4.cancelSingleton(H4P_TRID_MQRC);
            _discoDone=false;
            h4.dumpQ();            
//            subscribe(CSTR(string("all/").append(cmdhash())),0);
//            subscribe(CSTR(string(device+"/"+cmdhash())),0);
//            subscribe(CSTR(string(_cb[chipTag()]+"/"+cmdhash())),0);
//            subscribe(CSTR(string(_cb[boardTag()]+"/"+cmdhash())),0);
            publish("all/h4/mqtt/online",0,false,CSTR(device));
            h4pcConnected();
            Serial.printf("MQTT CONNECTED %d\n",b);
        });

        onDisconnect([this](AsyncMqttClientDisconnectReason reason){
            Serial.printf("MQTT DISCONNECT\n");
            if(!_discoDone){
                _discoDone=true;
                h4pcDisconnected();
                if(autorestart && WiFi.status()==WL_CONNECTED) {
                    Serial.printf("starting H4P_TRID_MQRC\n");
                    h4.every(H4MQ_RETRY,[this](){ start(); },nullptr,H4P_TRID_MQRC,true);
                    Serial.printf("H4P_TRID_MQRC started\n");
                    h4.dumpQ();
                }
            }
        });

        h4wifi.hookConnect([this](){ start(); });
        h4wifi.hookDisconnect([this](){ stop(); });
//        H4EVENT("H4P_AsyncMQTT::_hookIn");
    } else { DEPENDFAIL(wifi); }
}

uint32_t H4P_AsyncMQTT::_offline(vector<string> vs){
    return guard1(vs,[this](vector<string> vs){
        if(H4PAYLOAD!=device) _grid.erase(H4PAYLOAD);
        return H4_CMD_OK;
    }); 
}

uint32_t H4P_AsyncMQTT::_online(vector<string> vs){
    return guard1(vs,[this](vector<string> vs){
        if(H4PAYLOAD!=device) _grid.insert(H4PAYLOAD);
        return H4_CMD_OK;
    });    
}

void H4P_AsyncMQTT::_setup(){
    device=_cb[deviceTag()];
    setClientId(CSTR(device));
    setWill("all/h4/mqtt/offline",0,false,CSTR(device));

    string broker=_cb[brokerTag()];
    uint16_t port=atoi(CSTR(_cb[portTag()]));
    if(atoi(CSTR(broker))) {
        vector<string> vs=split(broker,".");
        setServer(IPAddress(PARAM_INT(0),PARAM_INT(1),PARAM_INT(2),PARAM_INT(3)),port);
    } else setServer(CSTR(broker),port);        
        
    setCredentials(CSTR(_cb["muser"]),CSTR(_cb["mpasswd"]));
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

void H4P_AsyncMQTT::subscribeDevice(string topic,H4_FN_MSG f){
    string fullTopic=device+"/"+topic;
    if(topic.back()=='#'){
        topic.pop_back();
        topic.pop_back();
    }
    h4sc.addCmd(topic,0,0,f);
    subscribe(CSTR(fullTopic),0);
}
void H4P_AsyncMQTT::unsubscribeDevice(string topic){
    string fullTopic=device+"/"+topic; // refactor
    if(topic.back()=='#'){
        topic.pop_back();
        topic.pop_back();
    }
    h4sc.removeCmd(topic);
    unsubscribe(CSTR(fullTopic));
}

void H4P_AsyncMQTT::start(){ 
    Serial.printf("MQTT start\n");
    if(!(WiFi.getMode() & WIFI_AP)) {
        Serial.printf("MQTT start wifi STA\n");
        autorestart=true;
        connect(); 
    }
    Serial.printf("MQTT start OUT\n");
}

void H4P_AsyncMQTT::stop(){
    Serial.printf("MQTT stop\n");
    if(!(WiFi.getMode() & WIFI_AP)) {
        Serial.printf("MQTT stop wifi STA\n");
        autorestart=false;
        disconnect(true);
    }
}
#endif