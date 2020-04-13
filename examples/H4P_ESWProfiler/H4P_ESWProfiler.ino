#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop
//
#ifdef ARDUINO_ARCH_ESP32
  #define D7 27
  #define D6 26
  #define D5 25
#endif

H4P_CmdErrors h4ce;


H4P_ExternalSqWave h4esw(D6,D5,99,49);
H4P_GPIOManager h4gm;

RawPin* pwm;

void h4setup() {    
    RawPin* pwm=h4gm.Raw(D7,INPUT,ACTIVE_HIGH,[](H4GPIOPin* p){});

    h4esw.fSweep(1000,60000,120000,500,[pwm](uint32_t f){
        Serial.print(millis());Serial.print(",");
        Serial.print(f);Serial.print(",");Serial.println(pwm->Rpeak/2);
        pwm->Rpeak=0;
    });
}
