#include<H4Plugins.h>
H4_USE_PLUGINS

#define F_POLL      5000

H4 h4(115200);
H4P_SerialCmd h4sc;
H4P_GPIOManager h4gm;

H4P_BinarySwitch h4onof(LED_BUILTIN,ACTIVE_LOW,OFF);

void h4setup(){
    h4gm.AnalogThresholdThing(LIGHT,F_POLL,100,H4GM_GREATER,&h4onof); // On if A0 > 100
    //h4gm.AnalogThresholdThing(LIGHT,F_POLL,100,H4GM_LESS,&h4onof); // On if A0 < 100
}
