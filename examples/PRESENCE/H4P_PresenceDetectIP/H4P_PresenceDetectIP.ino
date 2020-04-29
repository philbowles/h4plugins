#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

void onElvisDetect(bool b){
  Serial.printf("Elvis has %s the building\n",b ? "entered":"left");
}



H4P_GPIOManager h4gm;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","detector");
H4P_AsyncWebServer h4asws;



H4P_BinarySwitch h4onof(RELAY_BUILTIN,ACTIVE_HIGH,OFF);

H4P_IPDetector ippdElvis("Elvis","192.168.1.31",onElvisDetect);
H4P_IPDetectorSource ippdEinstein("Albert","192.168.1.32");
