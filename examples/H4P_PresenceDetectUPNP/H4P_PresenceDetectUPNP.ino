#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop


H4P_GPIOManager h4gm;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","h104");
H4P_AsyncWebServer h4asws;


H4P_BinarySwitch h4onof(RELAY_BUILTIN,ACTIVE_HIGH,OFF);

H4P_UPNPDetector upnpAndroidApp("WhiteAce","uuid:e4f4a646-a2cc-341c-899c-f576d3876287::upnp:rootdevice",[](bool b){ Serial.printf("WhiteAce %d\n",b); });
//H4P_UPNPDetectorSource upnpPhone("GreyAce","uuid:e4f4a646-a2cc-341c-899c-f576d3876287::upnp:rootdevice");
