#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop
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
  // 16 for nodeMCU - change it for your device
#define USER_BTN 16
#define UB_ACTIVE ACTIVE_LOW
#define UL_ACTIVE ACTIVE_LOW

#define U_DEBOUNCE  15

H4P_GPIOManager h4gm;
H4P_FlasherController h4fc;
H4P_BinarySwitch h4onof(LED_BUILTIN,UL_ACTIVE,OFF,[](bool b){
    Serial.print("STATE NOW ");Serial.println(b);
  });
H4P_MultiFunctionButton h4mfb(USER_BTN,INPUT_PULLUP,UB_ACTIVE,U_DEBOUNCE,LED_BUILTIN,UL_ACTIVE);

void onReboot(){
    Serial.println("Au Revoir");      
}
void onFactoryReset(){
    Serial.println("Adieu");   
}

void h4setup() { // H4 constructor starts Serial
    Serial.println("H4P_BinarySwitch 3-function Button Example v"H4P_VERSION);
    h4.once(5000,[](){ h4onof.turnOn(); });
    h4.once(10000,[](){ h4onof.turnOff(); });
    h4.once(15000,[](){ h4onof.toggle(); });
    h4.once(20000,[](){ h4onof.turn(0); });
}