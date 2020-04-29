
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_GPIOManager h4gm;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","fourteen");
H4P_AsyncWebServer h4asws;


H4P_BinarySwitch h4onof(RELAY_BUILTIN,ACTIVE_HIGH,OFF);
H4P_H4Detector h4pd6("six",[](bool b){ Serial.printf("SIX has %s the network\n",b ? "joined":"left"); });
