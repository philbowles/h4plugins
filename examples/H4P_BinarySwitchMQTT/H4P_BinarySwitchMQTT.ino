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
 * 
 * Also use an MQTT client to publish
 * h4plugins/h4/on
 * h4plugins/h4/off ... etc
 *  
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
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","h4plugins");
H4P_MQTT h4mqtt("192.168.1.4",1883);
H4P_AsyncWebServer h4asws("admin","admin");
H4P_BinarySwitch h4bs(LED_BUILTIN,UL_ACTIVE,OFF,[](bool b){
    Serial.print("STATE NOW ");Serial.println(b);
  });
H4P_ThreeFunctionButton h43fb(&h4bs,U_DEBOUNCE,USER_BTN,INPUT,UB_ACTIVE,LED_BUILTIN,UL_ACTIVE);

void onReboot(){
    Serial.println("About to reboot: Au Revoir");      
}

void onFactoryReset(){
    Serial.println("About to factory reset Adieu");   
}

void h4setup() { // H4 constructor starts Serial
    Serial.println("H4P_BinarySwitch 3-function Button Example v"H4P_VERSION);
    h4.once(5000,[](){ h4bs.turnOn(); });
    h4.once(10000,[](){ h4bs.turnOff(); });
    h4.once(15000,[](){ h4bs.toggle(); });
    h4.once(20000,[](){ h4bs.turn(0); });
}