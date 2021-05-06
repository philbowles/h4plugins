#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, larger Q size for scheduling, SerialCmd autostop

void onRTC(){
  h4ss.at(SUNRISE,OFF,[](bool b){ Serial.printf("COCK-A-DOODLE-DOO!"); });
  h4ss.atSource(SUNSET,ON); // BinarySwitch on @ sunset
}

H4P_SerialLogger h4sl;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX"); // device defaults to "H4-<chip id>"
H4P_Timekeeper h4tk("time.google.com","time1.google.com",2); // 2 cos France is GMT+2
H4P_Sunrise h4ss("48.4738943","-0.6029038"); // My French heaven
H4P_AsyncMQTT h4mqtt("http://192.168.1.4:1883");
H4P_BinarySwitch h4onof(RELAY_BUILTIN,ACTIVE_HIGH,OFF);
