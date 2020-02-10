#include <H4.h>
#include <H4P_SerialCmd.h>
#include <H4P_CmdErrors.h>
#include <H4P_WiFi.h>
#include <H4P_MQTT.h>

H4 h4(115200);
//
//  Callback is given a vector of strings (vs), each contains one component of input topic
//  NB last item is always payload, hence= vs.back()
//
//  If this sounds new, think of it like an array of Arduino Strings, (but with more functionality)
//
//  Assume user publishes cards/hearts with payload of "4"
//  In vector strings vs, you will find:
//  vs[0] = "hearts" - next "part" of topic after "/"
//  vs[1] = "4" - also known as vs.back() becuase its the last item. payload is ALWAYS vs.back()
//  In this case, vs.size() will = 2: a good indication that the input MAY be valid...1 or 3 CERTAINLY can't be
//
//  BUT BE WARNED!!!! 
//    Prepare to cope with nonsense, # will also allow cards/three-piece/with/waistcoat ["and tie"]
//  and you will get
//  In vector strings vs, you will find:
//  vs[0] = "three-piece" - next "part" of topic after "/"
//  vs[1] = "with" - next "part" of topic after "/"
//  vs[2] = "waistcoat" - next "part" of topic after "/"
//  vs[3] = "and tie" - also known as vs.back() becuase its the last item. payload is ALWAYS vs.back()
// In this case, vs.size() will = 4: you know immediatly its junk
//
//  Remember, the "s" in "vs" is "string": if there is  value you want to treat as an int, there are two useful MACROS:
//  PARAM( n ) and PAYLOAD_INT
//  So if you subscribe to something/# and someone publishes
//  something/with/an/int/42 and payload of 666 then
//  ...
//  ...
//  vs[4]="42"     in code, use int myInt=PARAM(4);    // 42
//  vs[5]="666"    in code use int myPayload=PAYLOAD_INT; // 666
// 
 void onMQTTConnect(){
    Serial.print("USER: MQTT connected\n");
    h4mqtt.subscribeDevice("cards/#",myCallback);
}

void onMQTTDisconnect(){
    Serial.print("USER: MQTT Disconnected\n");
}

H4P_SerialCmd h4sc;
H4P_CmdErrors h4ce;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","testbed");
H4P_MQTT h4mqtt("192.168.1.4",1883,"","",onMQTTConnect,onMQTTDisconnect); // no username / pword

uint32_t myCallback(vector<string> vs){
  for(auto const& v:vs) Serial.printf("v: %s\n",CSTR(v)); // show input
  if(vs.size()==2){
    string suit=vs[0];
    Serial.printf("Wildcard handler suit is %s, card is %s\n",(CSTR(suit)),CSTR(vs.back()));
    if(suit=="hearts" || suit=="clubs" || suit=="diamonds" || suit=="spades"){
      Serial.printf("You chose the %s of %s\n",CSTR(vs.back()),CSTR(suit));
      h4mqtt.publish("dealt",CSTR(string(vs.back()+" of "+suit)));
      return H4_CMD_OK;
    } 
    else {
      Serial.printf("Invalid suit %s\n",CSTR(suit));
      return H4_CMD_PAYLOAD_FORMAT;        
    }
  } 
  else {
    Serial.printf("Count of %d sub-items is obviously an error\n",vs.size());
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

    cards/... etc
*/
void h4setup() {
    Serial.println("\nH4 Plugins MQTT example v"H4P_VERSION);
}
/*
 Sample output
 
H4 Plugins MQTT example v0.0.1
USER: MQTT connected
v: hearts
v: 4
Wildcard handler suit is hearts, card is 4
You chose the 4 of hearts
v: hearts
v: Z
Wildcard handler suit is hearts, card is Z
You chose the Z of hearts
v: hearts
Count of 1 sub-items is obviously an error
scmd: Incorrect Payload Format cards/hearts
v: a
v: load
v: of
v: crap
Count of 4 sub-items is obviously an error
scmd: Incorrect Payload Format cards/a/load/of/crap

 */