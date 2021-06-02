#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) 

H4P_PinMachine pm;
H4P_BinarySwitch bs(LED_BUILTIN,H4P_ASSUMED_SENSE);

// OUTPUTS
std::vector<h4pOutput*> LEDS={
  new h4pOutput(D6,ACTIVE_HIGH),
  new h4pOutput(D7,ACTIVE_HIGH),
  new h4pOutput(D8,ACTIVE_HIGH),
};

h4pEncoderAuto rotary(D1,D2,INPUT_PULLUP,ACTIVE_HIGH,0,2,1,0,false);
h4pRetriggering pir(D0,INPUT,ACTIVE_HIGH,10000,new npPUBLISHONOFF);

void onGPIO(int pin,int value){
  if(pin==D1){
    pm.all(OFF);
    LEDS[value]->turnOn();
  }
}

void h4pGlobalEventHandler(const std::string& svc,H4PE_TYPE t,const std::string& msg){ switch(t){ H4P_FUNCTION_ADAPTER_GPIO; } }
