#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","testbed");
H4P_AsyncMQTT h4mqtt("http://192.168.1.4:1883"); // no username / pword

uint32_t myCallback(vector<string> vs){
  Serial.printf("USER: Msg received with %d parts and payload=%s\n",vs.size(),H4PAYLOAD.c_str()); // convert payload to C-style string
  dumpvs(vs);
  if(H4PAYLOAD=="good") {
      h4mqtt.publishDevice("goodpayload","Thank You!");
      return H4_CMD_OK;
  }
  else {
      h4mqtt.publishDevice("badpayload",H4PAYLOAD);
      return H4_CMD_PAYLOAD_FORMAT;
  }
}

void onMqttConnect(){
    h4mqtt.subscribeDevice("a",myCallback);
    h4mqtt.subscribeDevice("a/b",myCallback);
    h4mqtt.subscribeDevice("a/b/c",myCallback);
}

void onMqttDisconnect(){
    h4mqtt.unsubscribeDevice("a");
    h4mqtt.unsubscribeDevice("a/b");
    h4mqtt.unsubscribeDevice("a/b/c");
}

void h4pGlobalEventHandler(const string& svc,H4PE_TYPE t,const string& msg){
    switch(t){
        H4P_DEFAULT_SYSTEM_HANDLER
        case H4PE_SERVICE:
            H4P_SERVICE_ADAPTER(Mqtt);
            break;
    }
}