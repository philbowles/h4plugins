#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) 

#define USER_BTN 0
#define UB_ACTIVE ACTIVE_LOW

H4P_EventListener gpio(H4PE_GPIO,[](const std::string& pin,H4PE_TYPE t,const std::string& msg){
    int p=atoi(pin.c_str());
    int v=atoi(msg.c_str());
    switch(p){
      case USER_BTN:
        Serial.printf("P%d V%d\n",p,v);
        break;
    }
});
// every 5 seconds
h4pPolled ark(USER_BTN,INPUT_PULLUP,UB_ACTIVE,5000);