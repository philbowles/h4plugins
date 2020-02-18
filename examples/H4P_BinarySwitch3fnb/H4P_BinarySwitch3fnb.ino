#include<H4Plugins.h>
H4_USE_PLUGINS

H4 h4(115200);
/*
 * Try Serial commands
 * 
 * h4/on
 * h4/off
 * h4/toggle
 * h4/switch/n where n is 0 or 1
 * 
 * Also try short / medium / long press on USER_BTN
 */
#ifdef ARDUINO_ARCH_STM32
  #define UB_ACTIVE ACTIVE_HIGH
  #define UL_ACTIVE ACTIVE_HIGH
#else
  #define USER_BTN 0
  #define UB_ACTIVE ACTIVE_LOW
  #define UL_ACTIVE ACTIVE_LOW
#endif

#define U_DEBOUNCE  15

H4P_SerialCmd h4sc;
H4P_GPIOManager h4gm;
H4P_FlasherController h4fc;
H4P_BinarySwitch h4bs(LED_BUILTIN,UL_ACTIVE,OFF,[](bool b){
    Serial.print("STATE NOW ");Serial.println(b);
  });
H4P_ThreeFunctionButton h43fb(&h4bs,U_DEBOUNCE,USER_BTN,INPUT,UB_ACTIVE,LED_BUILTIN,UL_ACTIVE);

void onReboot(){
    Serial.println("Au Revoir");      
}
void onFactoryReset(){
    Serial.println("Adieu");   
}

void h4setup() { // H4 constructor starts Serial
    Serial.println("H4P_BinarySwitch 3-function Button Example v"H4P_VERSION);
    h4.once(5000,[](){ h4bs.turnOn(); });
    h4.once(10000,[](){ h4bs.turnOff(); });
    h4.once(15000,[](){ h4bs.toggle(); });
    h4.once(20000,[](){ h4bs.turn(0); });
}