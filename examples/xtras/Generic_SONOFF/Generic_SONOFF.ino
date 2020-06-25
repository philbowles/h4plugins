/*
SEE SONOFF_Basic

This is a "generic" version which omits device name and UPNP friendly name
This means it will come up as H4-<chip id> / upnp <chip id>
See ChezToiioT for how to auti-configure it on fitrst boot / after factory reset by using
retained MQTT messages and the h4/wifi/host2 command

*/
#include<H4Plugins.h>
H4_USE_PLUGINS(0,20,true) // Serial baud rate, Q size, SerialCmd autostop
H4P_GPIOManager h4gm;
H4P_FlasherController h4fc;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX"); // device defaults to "H4-<chip id>"
H4P_AsyncMQTT h4mqtt("192.168.1.4",1883);
H4P_AsyncWebServer h4asws;


H4P_BinarySwitch h4onof(RELAY_BUILTIN,ACTIVE_HIGH,OFF);
H4P_UPNPServer h4upnp; // friendly name defaults to "upnp <chip id>"
H4P_MultiFunctionButton h4mfb(BUTTON_BUILTIN,INPUT,ACTIVE_LOW,15,LED_BUILTIN,ACTIVE_LOW);
H4P_RemoteUpdate h4ru("http://192.168.1.4:1880/update");