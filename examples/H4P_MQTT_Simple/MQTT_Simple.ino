#include <H4.h>
#include <H4P_SerialCmd.h>
#include <H4P_CmdErrors.h>
#include <H4P_WiFi.h>
#include <H4P_MQTT.h>

H4 h4(115200);

void onMQTTConnect(){
    Serial.print("USER: MQTT connected\n");
    h4mqtt.subscribeDevice("mytopic",myCallback);
}

void onMQTTDisconnect(){
    Serial.print("USER: MQTT Disconnected\n");
}

H4P_SerialCmd h4sc;
H4P_CmdErrors h4ce;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","testbed");
H4P_MQTT h4mqtt("192.168.1.4",1883,"","",onMQTTConnect,onMQTTDisconnect); // no username / pword

uint32_t myCallback(vector<string> vs){
  Serial.printf("USER: Msg received with payload=%s\n",PAYLOAD.c_str()); // convert payload to C-style string
  if(PAYLOAD=="good") {
      h4mqtt.publishDevice("goodpayload","Thank You!");
      return H4_CMD_OK;
  }
  else {
      h4mqtt.publishDevice("badpayload",PAYLOAD);
      return H4_CMD_PAYLOAD_FORMAT;
  }
}

#ifdef ARDUINO_ARCH_STM32
  #define U_BOARD_NAME BOARD_NAME
#else
  #define U_BOARD_NAME ARDUINO_BOARD
#endif
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

    Then, publish topic

    testbed/mytopic with a payload of "good" and again with a different payload

*/
void h4setup() {
    Serial.println("\nH4 Plugins MQTT example v"H4P_VERSION);
}