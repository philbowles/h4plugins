#define H4P_VERBOSE 1
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_SerialLogger slog;

boolean condition=true;
bool conditionPredicate(){ return condition; }
// My test board has a buitin LED, it is assumed to be ACTIVE_LOW and blue in color
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","h4plugins");
H4P_ConditionalSwitch sw1(conditionPredicate,BUILTIN_LED,H4P_ASSUMED_SENSE,H4P_ASSUMED_COLOR);

void h4setup(){
    Serial.printf("H4P_ConditionalThing %s example\n",H4P_VERSION);
    Serial.printf("Send h4/on off toggle etc - after 1 minute, they will stop working\n");
    
    h4.once(60000,[]{
      condition=false;
      Serial.printf("Condition now false: h4/on off toggle etc will no longer work\n");
      sw1.syncCondition();
    });
}