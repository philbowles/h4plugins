#define H4P_VERBOSE 1
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop
//  Starts OFF, commanded ON after 5sec then turns OFF automatically after AUTO_OFF milliseconds

//
//  Function is called by
//  h4/off
//  h4/on
//  h4/toggle
//  h4/switch/n where n=0 or 1
//  And reports current state with h4/state
//  If MQTT is used, publishes current state 
//
#define AUTO_OFF  5000

H4P_SerialLogger h4sl;

// instead of just on/offing a GPIO like a Switch does, a thing calls a function
// the function does anything you want...and is on/offed by all the same features as a switch
H4P_BinaryThing h4onof([](bool b){ Serial.printf("I am now %s\n",b ? "ON":"OFF"); },OFF,AUTO_OFF);

void h4setup(){
  h4.every(15000,[]{ h4onof.turnOn(); });
}