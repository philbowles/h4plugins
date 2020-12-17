#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_LocalLogger h4ll(10000); // 10k = amount of SPIFFS to use ESP8266 / ESP32 only
H4P_SerialLogger h4sl;

void h4setup(){
  h4UserEvent("test1");
  h4UserEvent("Ztest2");
}