#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) 

#define USER_BTN 0
#define UB_ACTIVE ACTIVE_LOW

h4pOutput builtinLED(LED_BUILTIN, ACTIVE_LOW,OFF);

H4P_EventListener gpio(H4PE_GPIO,[](const std::string& pin,H4PE_TYPE t,const std::string& msg){
    int p=atoi(pin.c_str());
    int v=atoi(msg.c_str());
    switch(p){
      case USER_BTN:
        Serial.printf("P%d V%d\n",p,v);
        builtinLED.logicalWrite(v); // make LED match state of pin
        break;
    }
});

h4pFiltered tip(USER_BTN,INPUT,UB_ACTIVE,HIGH);