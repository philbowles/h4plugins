/*
 * Read documentation re using AP MODE first!!!
 * 
 * Edit config_plugins.h, set
 * 
 * #define H4P_USE_WIFI_AP          1
 * 
*/
#define H4P_VERBOSE 1
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

//H4P_SerialLogger slog(H4PE_ALMOST_ALL &~ H4PE_GPIO &~ H4PE_HEARTBEAT);
H4P_WiFi whiffy;
H4P_AsyncMQTT cutie;
H4P_LinkMaster lynx;
H4P_BinarySwitch h4onof(12,ACTIVE_HIGH);
H4P_UPNPServer h4upnp;
h4pMultifunctionButton h4mfb(0,INPUT,ACTIVE_LOW,15);
H4P_RemoteUpdate h4ru;