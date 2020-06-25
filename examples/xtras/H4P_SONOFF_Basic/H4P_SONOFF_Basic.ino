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
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop


H4P_GPIOManager h4gm;
H4P_FlasherController h4fc;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","eiffel");
H4P_AsyncMQTT h4mqtt("192.168.1.4",1883);
H4P_AsyncWebServer h4asws;


H4P_BinarySwitch h4onof(RELAY_BUILTIN,ACTIVE_HIGH,OFF);
H4P_UPNPServer h4upnp("Salon Eiffel Tower");
H4P_MultiFunctionButton h4mfb(BUTTON_BUILTIN,INPUT,ACTIVE_LOW,15,LED_BUILTIN,ACTIVE_LOW);