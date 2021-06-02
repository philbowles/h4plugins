#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) 

h4pTimed trial(0,INPUT_PULLUP,ACTIVE_LOW,20);

void h4pGlobalEventHandler(const std::string& svc,H4PE_TYPE t,const std::string& msg){
    switch(t){
        H4P_FUNCTION_ADAPTER_GPIO;
    }
}

void onGPIO(int pin,int value){ 
    Serial.printf("P%d V%d\n",pin,value);
}
