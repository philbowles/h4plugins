#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop
//  Starts OFF, commanded ON after 5sec then turns OFF automatically after AUTO_OFF milliseconds

#define AUTO_OFF  10000

H4P_SerialLogger h4sl;
H4P_BinaryThing h4onof([](bool b){ Serial.print("I am now ");Serial.println(b ? "ON":"OFF"); },ON,AUTO_OFF);

void h4setup(){
  h4.once(5000,[]{ h4onof.toggle(); }); // will turn off automatically after 10 seconds
}