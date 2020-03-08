#include<H4Plugins.h>
H4_USE_PLUGINS

H4 h4(115200);
H4P_SerialCmd h4sc;
#ifndef ARDUINO_ARCH_STM32
  H4P_LocalLogger h4ll(10000); // 10k = amount of SPIFFS to use ESP8266 / ESP32 only
#endif
H4P_SerialLogger h4sl;

void h4setup(){
  h4UserEvent("test1");
  h4UserEvent("Ztest2");
}