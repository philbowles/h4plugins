#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

boolean condition=true;

H4P_GPIOManager h4gm;

OutputPin* pCT=h4gm.Output(LED_BUILTIN,ACTIVE_LOW,OFF);

H4P_ConditionalThing h4onof([](bool b){ return condition; },[](bool b){
      Serial.printf("H4P_ConditionalThing %d [condition=%d]\n",b,condition);
      pCT->logicalWrite(b);
    });

void h4setup(){
    Serial.printf("H4P_ConditionalThing %s example\n",H4P_VERSION);
    Serial.printf("Send h4/on off toggle etc - after 1 minute, they will stop working\n");
    
    h4.once(60000,[]{
      condition=false;
      Serial.printf("Condition now false: h4/on off toggle etc will no longer work\n");
    });
}
