#include<H4Plugins.h>
H4_USE_PLUGINS(115200,10,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_LoopCount h4lc;

void h4setup(){
  Serial.println("send command h4/svc/stop/cmd and notice the difference");
}