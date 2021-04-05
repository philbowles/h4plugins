/*
 * Read documentation re using AP MODE first!!!
 * 
 * Edit config.h, set
 * 
 * #define H4P_USE_WIFI_AP          1
 * 
*/
#include<H4Plugins.h>
H4_USE_PLUGINS(0,10,true) // Serial baud rate, Q size, SerialCmd autostop

H4P_WiFi whiffy;
H4P_Heartbeat hb;
H4P_AsyncMQTT cutie;
H4P_BinarySwitch h4onof(RELAY_BUILTIN,ACTIVE_HIGH,OFF);
H4P_UPNPServer h4upnp;
h4pMultifunctionButton h4mfb(BUTTON_BUILTIN,INPUT,H4P_ASSUMED_SENSE,15);
H4P_RemoteUpdate h4ru(__FILE__);
