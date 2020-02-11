/*
The code is specifically designed for the popular Itead SONOFF Basic device.

It will however work on any ESP8266 / ESP32 device that has the following pin assignments:

LED_BUILTIN = a GPIO with some kind of LED output connected
BUTTON_BUILTIN = a GPIO connected to a "tact" type momentary button
RELAY_BUILTIN - a GPIO with some kind of ouput device connected. often this will be a relay but for demo purposes
it could be another LED or a buzzer...indeed anything that can give an audio or visual indication of an "ON" state from
simple GPIO HIGH/LOW will do.

and the appropriat "sense" settings of ACTIVE_HIGH/LOW for each

*/
#include<H4Plugins.h>
H4_USE_PLUGINS

H4 h4(115200);
H4P_SerialCmd h4sc;
H4P_GPIOManager h4gm;
H4P_FlasherController h4fc;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","h4plugins");
H4P_MQTT h4mqtt("192.168.1.4",1883);
H4P_AsyncWebServer h4asws("admin","admin");
H4P_UPNPSwitch h4upnp("Demo Switch",RELAY_BUILTIN,ACTIVE_HIGH,OFF);
H4P_ThreeFunctionButton h43fb(&h4upnp,15,BUTTON_BUILTIN,INPUT,ACTIVE_LOW,LED_BUILTIN,ACTIVE_LOW);