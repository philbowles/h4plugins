#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

#define F_POLL      5000
#define LIGHT       A0

H4P_GPIOManager h4gm;

H4P_BinarySwitch h4onof(LED_BUILTIN,ACTIVE_LOW,OFF);

void h4setup(){
    h4gm.AnalogThresholdSource(LIGHT,F_POLL,100,H4GM_GREATER); // On if A0 > 100
    //h4gm.AnalogThresholdSource(LIGHT,F_POLL,100,H4GM_LESS); // On if A0 < 100
}