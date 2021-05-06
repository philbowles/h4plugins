#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

boolean condition=true;
bool conditionPredicate(){ return condition; }

H4P_PinMachine pm;
h4pOutput builtin(LED_BUILTIN,H4P_ASSUMED_SENSE,OFF,H4P_ASSUMED_COLOR);

void myThingFunction(bool b){
  Serial.printf("I'm doing my thing %d\n",b);
  builtin.turn(b);
}

H4P_WiFi wiffy("XXXXXXXX","XXXXXXXX","picky");
H4P_AsyncMQTT asink("http://192.168.1.4:1883");
H4P_LinkMaster lm({"landing","master"});
H4P_ConditionalThing ct(myThingFunction,conditionPredicate);

void h4setup(){
    Serial.printf("H4P_ConditionalThing %s example\n",H4P_VERSION);
    Serial.printf("Send h4/on off toggle etc - after 1 minute, they will stop working\n");
    
    h4.once(60000,[]{
      condition=false;
      ct.syncCondition();
      Serial.printf("Condition now false: h4/on off toggle etc will no longer work\n");
      
    });
}