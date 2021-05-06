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
#define BUTTON_BUILTIN  0
#define RELAY_BUILTIN   12

#define H4P_VERBOSE 1
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_SerialLogger slog(H4PE_ALMOST_ALL &~ H4PE_GPIO);

H4P_WiFi wiffy("XXXXXXXX","XXXXXXXX","basic");
H4P_Heartbeat hb;
H4P_AsyncMQTT cutie("http://192.168.1.4:1883");
H4P_BinarySwitch ting(RELAY_BUILTIN,ACTIVE_HIGH,OFF);
H4P_UPNPServer lexy("Sonoff Basic Switch");
h4pMultifunctionButton mfb(BUTTON_BUILTIN,INPUT,H4P_ASSUMED_SENSE,15);