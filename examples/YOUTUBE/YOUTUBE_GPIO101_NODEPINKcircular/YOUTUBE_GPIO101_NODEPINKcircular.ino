#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false);

H4P_PinMachine pm;
// OUTPUTS
std::vector<h4pOutput*> LEDS={
  new h4pOutput(D6,ACTIVE_HIGH),
  new h4pOutput(D7,ACTIVE_HIGH),
  new h4pOutput(D8,ACTIVE_HIGH),
  new h4pOutput(LED_BUILTIN,ACTIVE_LOW)
};
// INPUT
h4pCircular stab(0,INPUT_PULLUP,ACTIVE_LOW,20,LEDS.size()); // count 1 to 4

void onGPIO(uint8_t pin,int value){
  if(pin==0 && value){
    pm.all(OFF);
    LEDS[value-1]->turnOn();
  }
}

void h4pGlobalEventHandler(const std::string& svc,H4PE_TYPE t,const std::string& msg){
  switch(t){
    H4P_FUNCTION_ADAPTER_GPIO;
  }
}
