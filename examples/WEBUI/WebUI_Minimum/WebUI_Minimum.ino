#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","uiminimum");
H4P_AsyncWebServer h4asws;
