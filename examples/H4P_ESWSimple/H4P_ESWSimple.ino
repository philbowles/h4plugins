#include<H4Plugins.h>
H4_USE_PLUGINS

H4 h4(115200);

// Makes it easy to switch between Wemos D1 mini and Wemos LOLIN D32
#ifdef ARDUINO_ARCH_ESP32
  #define D6 26
  #define D5 25
#endif

H4P_CmdErrors h4ce;
H4P_SerialCmd h4sc;
H4P_ExternalSqWave h4esw(D6,D5,100,50); // start @ F=100Hz, DC = 50% 

void h4setup() {
  Serial.println("\nExternal Square Wave example v"H4P_VERSION);
  // delay long enough to show initial values, then sweep
  h4.once(10000,[](){
    Serial.println("Starting F sweep from 60khz to 120kHz");
    Serial.println("Increasing value by 500Hz every 1000 milliseconds (1sec)");
    
    h4esw.fSweep(1000,60000,120000,500,[](uint32_t f){
      Serial.print(millis());Serial.print(" F=");Serial.println(f);
    });
  });
}
