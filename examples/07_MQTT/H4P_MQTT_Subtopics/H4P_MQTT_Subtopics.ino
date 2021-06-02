#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","testbed");
H4P_AsyncMQTT h4mqtt("http://192.168.1.4:1883"); // no username / pword

uint32_t myCallback(std::vector<std::string> vs){
  Serial.printf("USER: Msg received with %d parts and payload=%s\n",vs.size(),H4PAYLOAD.c_str()); // convert payload to C-style string
  if(H4PAYLOAD=="good") {
      h4mqtt.publishDevice("goodpayload","Thank You!");
      return H4_CMD_OK;
  }
  else {
      h4mqtt.publishDevice("badpayload",H4PAYLOAD);
      return H4_CMD_PAYLOAD_FORMAT;
  }
}

void onMqttConnect(){ h4mqtt.subscribeDevice({"a","a/b","a/b/c"},myCallback); }

void onMqttDisconnect(){}

void h4pGlobalEventHandler(const std::string& svc,H4PE_TYPE t,const std::string& msg){
    switch(t){
        H4P_DEFAULT_SYSTEM_HANDLER
        case H4PE_SERVICE:
            H4P_SERVICE_ADAPTER(Mqtt);
            break;
    }
}