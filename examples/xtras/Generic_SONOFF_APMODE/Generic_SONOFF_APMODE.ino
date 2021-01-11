/*
This is a "generic" version which omits device name and UPNP friendly name
This means it will come up as H4-<chip id> / H4 Device <chip id>
See NODE-RED server flows for how to auto-configure it on first boot / after factory reset and the h4/wifi/host2 command
*/
#define H4P_USE_AP
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,5,false) // Serial baud rate, Q size, SerialCmd autostop
//
//    Configuration
//
const char* MQTT_SERVER="192.168.1.4";
const int   MQTT_PORT=1883;
const char* REMOTE_UPDATE_URL="192.168.1.4:1880/update";

H4P_GPIOManager h4gm;
H4P_FlasherController h4fc;
H4P_WiFiAP h4wifi;
H4P_AsyncMQTT h4mqtt(MQTT_SERVER,MQTT_PORT);
H4P_AsyncWebServer h4asws;
H4P_BinarySwitch h4onof(RELAY_BUILTIN,ACTIVE_HIGH,OFF);
H4P_UPNPServer h4upnp; // friendly name defaults to "upnp <chip id>"
H4P_MultiFunctionButton h4mfb(BUTTON_BUILTIN,INPUT,ACTIVE_LOW,15,LED_BUILTIN,ACTIVE_LOW);
H4P_RemoteUpdate h4ru(REMOTE_UPDATE_URL,__FILE__);