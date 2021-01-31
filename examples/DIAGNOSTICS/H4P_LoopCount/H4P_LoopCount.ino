/*
 * N.B. this will only work if H4 has loop counting compiled in
 * see H4 for documentation for details
 */
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_SerialLogger h4sl;
H4P_EmitLoopCount elc;

void h4setup(){
  Serial.println("send command h4/svc/stop/cmd and notice the difference");
}