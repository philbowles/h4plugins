#define H4P_VERBOSE 1
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop
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
#define USER_BTN 0
#define UB_ACTIVE ACTIVE_LOW
#define U_DEBOUNCE  15

void h4pGlobalEventHandler(const std::string& svc,H4PE_TYPE t,const std::string& msg){
  switch(t){
    case H4PE_REBOOT:
      Serial.printf("App is about to reboot: save config (if any) etc\n");
      break;
    case H4PE_FACTORY:
      Serial.printf("App is about to 'Factory Reset': clear any saved config data etc\n");
      break;
  }
}

H4P_PinMachine h4gm;
H4P_Signaller h4fc;

H4P_BinarySwitch h4onof(LED_BUILTIN,H4P_ASSUMED_SENSE);
h4pMultifunctionButton h4mfb(USER_BTN,INPUT_PULLUP,UB_ACTIVE,U_DEBOUNCE);

void h4setup() { // H4 constructor starts Serial
    Serial.println("\nH4P_BinarySwitch multifunction Button Example v"H4P_VERSION);
    h4.once(5000,[](){ h4onof.turnOn(); });
    h4.once(10000,[](){ h4onof.turnOff(); });
    h4.once(15000,[](){ h4onof.toggle(); });
    h4.once(20000,[](){ h4onof.turn(0); });
}