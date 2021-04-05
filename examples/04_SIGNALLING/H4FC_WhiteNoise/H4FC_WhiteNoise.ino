#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_Signaller h4fc;
/*
Use for active-low LEDs e.g. ESP8266, ESP32.

runs forever making LED pulsae randomly
*/
#define ULED_ACTIVE ACTIVE_LOW

void h4setup() { h4.everyRandom(50,150,[](){ h4fc.pulseLED(random(20,120),LED_BUILTIN,ULED_ACTIVE); }); }