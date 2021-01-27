#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_WiFi whiffy("XXXXXXXX","XXXXXXXX","gangmaster");
H4P_Heartbeat buddyholly;
H4P_AsyncMQTT cutie("192.168.1.20",1883);
H4P_BinarySwitch h4onof(RELAY_BUILTIN,ACTIVE_HIGH,OFF);
H4P_UPNPServer alexa("Plugins Gangmaster");
H4P_Gangmaster capone;