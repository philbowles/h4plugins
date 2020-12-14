#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

boolean condition=true;

H4P_GPIOManager h4gm;

H4P_ConditionalSwitch h4onof(LED_BUILTIN,ACTIVE_LOW,OFF,[](bool b){ return condition; });

void h4setup(){
    Serial.printf("%s %s example\n",__FILE__,H4P_VERSION);
    Serial.printf("Send h4/on off toggle etc - after 1 minute, they will stop working\n");
    
    h4.once(60000,[]{
      condition=false;
      Serial.printf("Condition now false: h4/on off toggle etc will no longer work\n");
    });
}