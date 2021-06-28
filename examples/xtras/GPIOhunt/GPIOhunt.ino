#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false);

H4_TIMER pulsar=nullptr;
H4P_PinMachine pm;
H4P_Signaller fc;
H4P_WiFi wiffy("XXXXXXXX","XXXXXXXX","lightrider");
// INPUTS
h4pEncoderAuto ea(D1,D2,INPUT,ACTIVE_HIGH,0,3,1,0,true); // false = no wrap
h4pLatching latch(0,INPUT_PULLUP,ACTIVE_LOW,15); // 15 ms of debounce
// OUTPUTS
std::vector<h4pOutput*> LEDS={
  new h4pOutput(D6,ACTIVE_HIGH,H4P_UILED_RED),
  new h4pOutput(D7,ACTIVE_HIGH,OFF,H4P_UILED_ORANGE),
  new h4pOutput(D8,ACTIVE_HIGH,OFF,H4P_UILED_GREEN),
  new h4pOutput(LED_BUILTIN,ACTIVE_LOW,OFF,H4P_UILED_BLUE)
};

void h4pGlobalEventHandler(const std::string& svc,H4PE_TYPE t,const std::string& msg){
  switch(t){
    H4P_FUNCTION_ADAPTER_GPIO;
  }
}

void stopEverything(int value){
    fc.stopAll(); // any/all flashers
    h4.cancel(pulsar); // any/all twinklers
    pm.all(value); // any/all solid on/off 
}

void onGPIO(uint8_t pin,int value){
  static int style=-1;  
  switch(pin){
    case 0:
      stopEverything(value);
      if(!value){
        style++;
        if(style>3) style=0;
        h4.nTimes(3,200,[=]{ fc.pulsePin(250,LEDS[style]); });
      }
      break;
    case D1: // encoder
      stopEverything(OFF);
      switch(style){
        case 0:
          LEDS[value]->turn(ON);
          break;
        case 1:
          fc.flashPin(500,LEDS[value]);
          break;
        case 2:
          pulsar=h4.everyRandom(75,500,[=]{ fc.pulsePin(random(15,35),LEDS[value]); });
          break;
        case 3:
          fc.throbPin(500,LEDS[value]);
          break;
        default:
          style=0;
          break;
      }
      break;
  }
}
