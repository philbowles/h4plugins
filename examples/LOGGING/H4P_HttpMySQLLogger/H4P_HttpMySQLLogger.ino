#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop


H4P_SerialLogger h4sl;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","mysqllogger");
H4P_HttpMySQLLogger h4mysql("http://192.168.1.20:8266/"); // trailing slash!!!

uint32_t serno;

void h4setup(){
  h4UserEvent("normal call");
  h4UserEvent("test1"); 
  h4.every(1000,[](){ h4UserEvent("%u",serno++); });
}