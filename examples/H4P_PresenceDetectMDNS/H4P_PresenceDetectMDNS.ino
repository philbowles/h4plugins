#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop


H4P_GPIOManager h4gm;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","fourteen");
H4P_AsyncMQTT h4mqtt("192.168.1.4",1883);
H4P_AsyncWebServer h4asws;


H4P_BinarySwitch h4onof(RELAY_BUILTIN,ACTIVE_HIGH,OFF);
H4P_MDNSDetector h4mdb("BLACKBOX","http","tcp",[](bool b){ Serial.printf("BLACKBOX %d\n",b); });
