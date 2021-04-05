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
 * 
 * Also use an MQTT client to publish
 * h4plugins/h4/on
 * h4plugins/h4/off ... etc
 *  
 */
#define USER_BTN 0
#define UB_ACTIVE ACTIVE_LOW
#define UL_ACTIVE ACTIVE_LOW

#define U_DEBOUNCE  15

H4P_PinMachine h4gm;
H4P_Signaller h4fc;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","h4plugins");
H4P_AsyncMQTT h4mqtt("192.168.1.4",1883);

H4P_BinarySwitch h4onof(LED_BUILTIN,UL_ACTIVE,OFF,[](bool b){
    Serial.print("STATE NOW ");Serial.println(b);
  });
h4pMultifunctionButton h4mfb(USER_BTN,INPUT_PULLUP,UB_ACTIVE,U_DEBOUNCE);

void onReboot(){
    Serial.println("About to reboot: Au Revoir");      
}

void onFactoryReset(){
    Serial.println("About to factory reset Adieu");   
}

void h4setup() { // H4 constructor starts Serial
    Serial.println("H4P_BinarySwitch MQTT Example v"H4P_VERSION);
    h4.once(5000,[](){ h4onof.turnOn(); });
    h4.once(10000,[](){ h4onof.turnOff(); });
    h4.once(15000,[](){ h4onof.toggle(); });
    h4.once(20000,[](){ h4onof.turn(0); });
}