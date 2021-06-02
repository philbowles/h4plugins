#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) 

H4P_EventListener gpio(H4PE_GPIO,[](const std::string& pin,H4PE_TYPE t,const std::string& msg){
    int p=atoi(pin.c_str());
    int v=atoi(msg.c_str());
    switch(p){
      case D1:
        Serial.printf("P%d V = %d\n",p,v);
        break;
    }
});

h4pEncoder rotary(D1,D2);
