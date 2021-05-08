#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop
//
//  Function is called by
//  h4/off
//  h4/on
//  h4/toggle
//  h4/switch/n where n=0 or 1
//

// instead of just on/offing a GPIO like a Switch does, a thing calls a function
// the function does anything you want...and is on/off'ed by all the same features as a switch
H4P_BinaryThing h4onof([](bool b){ Serial.printf("I am now %s\n",b ? "ON":"OFF"); });

void h4setup(){
  h4.every(10000,[]{ h4onof.toggle(); });
}