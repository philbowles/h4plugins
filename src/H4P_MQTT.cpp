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
#include<H4P_MQTT.h>
#ifndef H4P_NO_WIFI
uint32_t H4P_MQTT::_change(vector<string> vs){
    return guardString2(vs,[this](string a,string b){ 
        if(isNumeric(b)){
            change(a,atoi(CSTR(b))); 
        }
    });
}

void H4P_MQTT::_forceDisconnect(bool autorestart){    
    disconnect();
    h4.cancelSingleton({H4P_TRID_MQTT,H4P_TRID_MQRC});
    if(!_discoDone){
        h4pcDisconnected();   
        _discoDone=true;
        H4EVENT(stringFromInt(state()),mqttTag());
    }
    if(autorestart && WiFi.status()==WL_CONNECTED) start();
}

void H4P_MQTT::_hookIn() {
    if(H4Plugin::isLoaded(wifiTag()) ){
        _setup(); 
        h4wifi.hookConnect([this](){ start(); });
        h4wifi.hookDisconnect([this](){ _forceDisconnect(); });
    } else { DEPENDFAIL(wifi); }
}

uint32_t H4P_MQTT::_offline(vector<string> vs){
    return guard1(vs,[this](vector<string> vs){
        if(PAYLOAD!=device) _grid.erase(PAYLOAD);
        return H4_CMD_OK;
    }); 
}

uint32_t H4P_MQTT::_online(vector<string> vs){
    return guard1(vs,[this](vector<string> vs){
        if(PAYLOAD!=device) _grid.insert(PAYLOAD);
        return H4_CMD_OK;
    });    
}

void H4P_MQTT::_setup(){
    setClient(_wifiClient);
    string broker=_cb[brokerTag()];
    uint16_t port=atoi(CSTR(_cb[portTag()]));
    if(atoi(CSTR(broker))) {
        vector<string> vs=split(broker,".");
        setServer(IPAddress(PARAM_INT(0),PARAM_INT(1),PARAM_INT(2),PARAM_INT(3)),port);
    } else setServer(CSTR(broker),port);        
        
    setCallback([](char* topic, byte* payload, unsigned int length){
    h4.queueFunction(
        bind([](string topic, string pload){ 
            h4sc._executeCmd(CSTR(string(mqttTag()).append("/").append(topic)),pload); 
        },string(topic),stringFromBuff(payload,length)),nullptr,H4P_TRID_MQMS);
    });	
}

void H4P_MQTT::change(const string& broker,uint16_t port){
    stop();
    _wifiClient.stop();
    _cb[brokerTag()]=broker;
    _cb[portTag()]=stringFromInt(port);
    _setup();
    start();
}

void H4P_MQTT::publishDevice(const string& topic,const string& payload){ publish(CSTR(string("h4/"+device+"/"+topic)),CSTR(payload)); }

void H4P_MQTT::subscribeDevice(string topic,H4_FN_MSG f){
    string fullTopic=device+"/"+topic;
    if(topic.back()=='#'){
        topic.pop_back();
        topic.pop_back();
    }
    h4sc.addCmd(topic,0,0,f);
    subscribe(CSTR(fullTopic));
}
void H4P_MQTT::unsubscribeDevice(string topic){
    string fullTopic=device+"/"+topic; // refactor
    if(topic.back()=='#'){
        topic.pop_back();
        topic.pop_back();
    }
    h4sc.removeCmd(topic);
    unsubscribe(CSTR(fullTopic));
}
/*
-4 : MQTT_CONNECTION_TIMEOUT - the server didn't respond within the keepalive time
-3 : MQTT_CONNECTION_LOST - the network connection was broken
-2 : MQTT_CONNECT_FAILED - the network connection failed
-1 : MQTT_DISCONNECTED - the client is disconnected cleanly
0 : MQTT_CONNECTED - the client is connected
1 : MQTT_CONNECT_BAD_PROTOCOL - the server doesn't support the requested version of MQTT
2 : MQTT_CONNECT_BAD_CLIENT_ID - the server rejected the client identifier
3 : MQTT_CONNECT_UNAVAILABLE - the server was unable to accept the connection
4 : MQTT_CONNECT_BAD_CREDENTIALS - the username/password were rejected
5 : MQTT_CONNECT_UNAUTHORIZED - the client was not authorized to connect
*/
void H4P_MQTT::start(){
    if(!(WiFi.getMode() & WIFI_AP)){
        device=_cb[deviceTag()];
        if(connect( CSTR(device),
                    CSTR(_cb["muser"]),
                    CSTR(_cb["mpasswd"]),
                    "all/h4/mqtt/offline",
                    0,
                    false,
                    CSTR(device))){
            _discoDone=false;
            h4.cancelSingleton({H4P_TRID_MQTT,H4P_TRID_MQRC});
            h4.every(H4MQ_MQTT_RATE,[this](){ 
                if(!loop()) {
                _forceDisconnect();
                }
            },nullptr,H4P_TRID_MQTT,true);
            subscribe(CSTR(string("all/").append(cmdhash())));
            subscribe(CSTR(string(device+"/"+cmdhash())));
            subscribe(CSTR(string(_cb[chipTag()]+"/"+cmdhash())));
            subscribe(CSTR(string(_cb[boardTag()]+"/"+cmdhash())));
            publish("all/h4/mqtt/online",CSTR(device));
            h4pcConnected();
        } else h4.once(H4MQ_RETRY,[this](){ start(); },nullptr,H4P_TRID_MQRC,true);
    }
}
#endif