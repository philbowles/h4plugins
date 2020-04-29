#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_LoopCount h4lc;

void h4setup(){
  Serial.println("send command h4/svc/stop/scmd and notice the difference");
}