#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) 

H4P_EventListener gpio(H4PE_GPIO,[](const std::string& pin,H4PE_TYPE t,const std::string& msg){
    int p=atoi(pin.c_str());
    int v=atoi(msg.c_str());
    switch(p){
      case A0:
        Serial.printf("P%d Value=%d\n",p,v);
        break;
    }
});

h4pGPIO diy(A0,INPUT,H4P_ASSUMED_SENSE,{
    npPassTimer,
    new npROLLINGAVERAGE,
    new npLOPASS{230}, // only pass through values < 230
// or try:
//  new npHIPASS{225},  // only > 225
//  new npBANDPASS{150,1000}, // pass only > 150 && < 1000
//  new npNOTCH{150,1000}, // only < 150 || > 1000
    new npVALUEDIFF{5}, // pass only if changed by more than +/- 5 since last time
    npPublishValue
});

void h4setup(){ H4P_PinMachine::periodicRead(A0,1000); }