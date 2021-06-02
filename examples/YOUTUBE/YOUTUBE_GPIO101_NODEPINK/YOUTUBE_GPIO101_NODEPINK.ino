#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false);

H4P_PinMachine pm;
// INPUT
h4pTactless stab(0,INPUT_PULLUP,ACTIVE_LOW,20);
// OUTPUTS
std::vector<h4pOutput*> LEDS={
  new h4pOutput(D6,ACTIVE_HIGH),
  new h4pOutput(D7,ACTIVE_HIGH),
  new h4pOutput(D8,ACTIVE_HIGH),
  new h4pOutput(LED_BUILTIN,ACTIVE_LOW)
};

void onGPIO(uint8_t pin,int value){
  static int count=0;  
  if(pin==0){
    pm.all(OFF);
    LEDS[(count++)%LEDS.size()]->turnOn();
  }
}

void h4pGlobalEventHandler(const std::string& svc,H4PE_TYPE t,const std::string& msg){
  switch(t){
    H4P_FUNCTION_ADAPTER_GPIO;
  }
}
