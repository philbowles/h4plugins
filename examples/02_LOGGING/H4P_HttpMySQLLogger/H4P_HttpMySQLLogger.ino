#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_SerialLogger h4sl;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","mysqllogger");
H4P_HttpMySQLLogger h4mysql("http://192.168.1.20:8266/"); // trailing slash!!!

uint32_t serno;

void h4setup(){
  static uint32_t pingCount=0;
  h4.once(3000,[](){ 
    h4pUserEvent("Evens %d",666); 
    h4pUserEvent("Odds %d",669); 
  });
  h4.every(1000,[](){ h4pUserEvent("%d",pingCount++); }); // sends H4PE_MSG, so will get picked up
  h4.every(5000,[](){ h4psysevent("user",H4PE_SYSINFO,"ignore me %d",millis()); }); // will get ignored by YOU
}