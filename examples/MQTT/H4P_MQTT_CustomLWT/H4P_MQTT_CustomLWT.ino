#include<H4Plugins.h>
H4_USE_PLUGINS(115200,10,false) // Serial baud rate, Q size, SerialCmd autostop

// necessary forward declarations
void onMQTTConnect();
void onMQTTDisconnect();

struct H4P_LWT myLWT={"myLWT","byebye",1,false}; // topic,pload,QoS,retain

H4P_VerboseMessages h4vm;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","testbed");
H4P_AsyncMQTT h4mqtt("192.168.1.4",1883,"","",onMQTTConnect,onMQTTDisconnect,myLWT); // Custom lwt
//H4P_AsyncMQTT h4mqtt("192.168.1.4",1883,"","",onMQTTConnect,onMQTTDisconnect); // NO Custom lwt

uint32_t myCallback(vector<string> vs){
  Serial.printf("USER: Msg received with payload=%s\n",H4PAYLOAD.c_str()); // convert payload to C-style string
  if(H4PAYLOAD=="good") {
      h4mqtt.publishDevice("goodpayload","Thank You!");
      return H4_CMD_OK;
  }
  else {
      h4mqtt.publishDevice("badpayload",H4PAYLOAD);
      return H4_CMD_PAYLOAD_FORMAT;
  }
}
void onMQTTConnect(){
    Serial.print("USER: MQTT connected\n");
    h4mqtt.subscribeDevice("mytopic",myCallback);
}

void onMQTTDisconnect(){
    Serial.print("USER: MQTT Disconnected\n");
}