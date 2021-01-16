/*
SEE SONOFF_Basic

This is a "generic" version which omits device name and UPNP friendly name
This means it will come up as H4-<chip id> / upnp <chip id>
See ChezToiioT for how to auti-configure it on fitrst boot / after factory reset by using
retained MQTT messages and the h4/wifi/host2 command
*/
#include<H4Plugins.h>
H4_USE_PLUGINS(0,20,true) // Serial baud rate, Q size, SerialCmd autostop
//
//    Configuration
//
const char* WIFI_SSID="XXXXXXXX";
const char* WIFI_PASS="XXXXXXXX";
const char* MQTT_SERVER="192.168.1.4";
const int   MQTT_PORT=1883;
const char* REMOTE_UPDATE_URL="192.168.1.4:1880/update";

H4P_GPIOManager h4gm;
H4P_FlasherController h4fc;
H4P_WiFi h4wifi(WIFI_SSID,WIFI_PASS);
H4P_AsyncMQTT h4mqtt(MQTT_SERVER,MQTT_PORT);
H4P_BinarySwitch h4onof(RELAY_BUILTIN,ACTIVE_HIGH,OFF);
H4P_UPNPServer h4upnp; // friendly name defaults to "upnp <chip id>"
H4P_MultiFunctionButton h4mfb(BUTTON_BUILTIN,INPUT,ACTIVE_LOW,15,LED_BUILTIN,ACTIVE_LOW);
H4P_RemoteUpdate h4ru(REMOTE_UPDATE_URL,__FILE__);
