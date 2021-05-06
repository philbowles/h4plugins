#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop
//
// See https://github.com/philbowles/mysqlrest for a webserver that will
// store the events being logged here 
//
H4P_SerialLogger h4sl;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","mysqllogger");
H4P_RemoteLogger dillo("http://192.168.1.20:8266");

void h4setup(){
  static uint32_t pingCount=0;
  h4.once(10000,[](){ h4pUserEvent("Evens %d",random(0,100)*2); });
  h4.every(1000,[](){ h4pUserEvent("%d",pingCount++); });
  h4.every(5000,[](){ h4psysevent("user",H4PE_SYSINFO,"heap %u",_HAL_freeHeap()); });
}