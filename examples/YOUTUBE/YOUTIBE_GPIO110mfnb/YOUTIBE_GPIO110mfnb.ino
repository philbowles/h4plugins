#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) 
H4P_Signaller sig; // needed by mfnb
h4pOutput* redLED=new h4pOutput(D6, ACTIVE_HIGH);

h4pMultifunctionButton mfb(0,INPUT_PULLUP,ACTIVE_LOW,20);
void h4pGlobalEventHandler(const std::string& svc,H4PE_TYPE t,const std::string& msg){
    switch(t){
      case H4PE_REBOOT:
        Serial.printf("REBOOT: As Arnie said: I'll be back!\n");
        break;
      case H4PE_FACTORY:
        Serial.printf("FACTORY: Morituri Te Salutant!\n");
        break;
      case H4PE_GVCHANGE:
        if(svc=="state"){
          auto i=atoi(msg.data()); 
          Serial.printf("state change %d\n",i); // < 2 sec
          redLED->turn(i);
        }      
        break;
    }
}
