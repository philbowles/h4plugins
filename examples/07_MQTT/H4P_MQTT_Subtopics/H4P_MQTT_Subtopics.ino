#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

// necessary forward declarations
void onMQTTConnect();
void onMQTTDisconnect();

H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","testbed");
H4P_AsyncMQTT h4mqtt("192.168.1.4",1883,"","",onMQTTConnect,onMQTTDisconnect); // no username / pword

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

void onMQTTConnect(){
    Serial.print("USER: MQTT connected\n");
    h4mqtt.subscribeDevice("a",myCallback);
    h4mqtt.subscribeDevice("a/b",myCallback);
    h4mqtt.subscribeDevice("a/b/c",myCallback);
}

void onMQTTDisconnect(){
    Serial.print("USER: MQTT Disconnected\n");
    h4mqtt.unsubscribeDevice("a");
    h4mqtt.unsubscribeDevice("a/b");
    h4mqtt.unsubscribeDevice("a/b/c");
}