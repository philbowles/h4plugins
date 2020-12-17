#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

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



H4P_CmdErrors h4ce;
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

#define U_BOARD_NAME ARDUINO_BOARD
/*
    Open Serial monitor and try typing any of the following:

    h4/show/mqtt
    h4/mqtt/change // payload = newbroker,newport,newusername,newpassword
    h4/mqtt/grid // show all local H4 devices
    h4/mqtt/restart
    h4/mqtt/start
    h4/mqtt/stop

    Using any MQTT client MQTTSpy, NODE RED etc etc, subscribe to testbed/# 
    to see all the messages from this device:

    Then, publish topics

    testbed/a
    testbed/a/b
    testbed/a/b/c
*/
void h4setup() {
    Serial.println("\nH4 Plugins MQTT subtopics example v"H4P_VERSION);
}