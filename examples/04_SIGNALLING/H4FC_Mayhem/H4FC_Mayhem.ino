#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_Signaller h4fc;

#define ULED_ACTIVE ACTIVE_LOW

#define LED_RED D6
#define LED_AMBER D7
#define LED_GREEN D8

#define FLASH_LO  50
#define FLASH_HI  150
#define PULSE_LO  10
#define PULSE_HI  140


void h4setup() {
  h4fc.flashPin(1000,LED_BUILTIN,ULED_ACTIVE);
  h4.everyRandom(FLASH_LO,FLASH_HI,[](){ h4fc.pulseLED(random(PULSE_LO,PULSE_HI),LED_RED,ACTIVE_HIGH); });  
  h4.everyRandom(FLASH_LO,FLASH_HI,[](){ h4fc.pulseLED(random(PULSE_LO,PULSE_HI),LED_AMBER,ACTIVE_HIGH); });  
  h4.everyRandom(FLASH_LO,FLASH_HI,[](){ h4fc.pulseLED(random(PULSE_LO,PULSE_HI),LED_GREEN,ACTIVE_HIGH); });
}