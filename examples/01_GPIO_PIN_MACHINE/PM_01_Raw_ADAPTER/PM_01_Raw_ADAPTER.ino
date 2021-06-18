#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) 

#define USER_BTN 0
#define UB_ACTIVE ACTIVE_LOW

h4pOutput builtinLED(LED_BUILTIN, H4P_ASSUMED_SENSE,OFF);

void h4pGlobalEventHandler(const std::string& svc,H4PE_TYPE t,const std::string& msg){
  switch(t){
    H4P_FUNCTION_ADAPTER_GPIO;
  }   
}

void onGPIO(int pin,int value){ // int, int: use H4P_FUNCTION_ADAPTER_II
    switch(pin){
      case USER_BTN:
        Serial.printf("RAW P%d V%d\n",pin,value);
        builtinLED.logicalWrite(value); // make LED match state of pin
        break;
    }
}

h4pRaw raw(USER_BTN,INPUT,UB_ACTIVE);