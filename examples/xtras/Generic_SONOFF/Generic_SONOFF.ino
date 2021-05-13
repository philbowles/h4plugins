//#define H4P_VERBOSE 1
#include<H4Plugins.h>
H4_USE_PLUGINS(0,20,true) // Serial baud rate, Q size, SerialCmd autostop

const char* WIFI_SSID="XXXXXXXX";
const char* WIFI_PASS="XXXXXXXX";
const char* MQTT_SERVER="http://192.168.1.4:1883";
const char* REMOTE_UPDATE_URL="192.168.1.4:1880/update";

#define RELAY_BUILTIN 12
#define BUTTON_BUILTIN 0

//H4P_SerialLogger slog(H4PE_ALMOST_ALL);
H4P_WiFi wiffy(WIFI_SSID,WIFI_PASS);
H4P_AsyncMQTT cutie(MQTT_SERVER); // will default to <router IP>:1883
H4P_LinkMaster  lynx;
H4P_BinarySwitch bs(RELAY_BUILTIN,ACTIVE_HIGH);
H4P_UPNPServer h4upnp; // friendly name defaults to "upnp <chip id>"
h4pMultifunctionButton h4mfb(BUTTON_BUILTIN,INPUT,ACTIVE_LOW,15);
H4P_RemoteUpdate h4ru(REMOTE_UPDATE_URL);