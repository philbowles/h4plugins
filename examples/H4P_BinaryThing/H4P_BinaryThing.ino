#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop
//
//  Function is called by
//  h4/off
//  h4/on
//  h4/toggle
//  h4/switch/n where n=0 or 1
//  And reports current state with h4/state
//  If MQTT is used, publishes current state 
//




H4P_BinaryThing h4onof([](bool b){ Serial.print("I am now ");Serial.println(b ? "ON":"OFF"); });

void h4setup(){
  h4.every(10000,[]{ h4onof.toggle(); });
}