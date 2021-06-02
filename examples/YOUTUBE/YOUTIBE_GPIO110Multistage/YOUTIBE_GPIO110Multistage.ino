#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) 

H4P_Signaller sig;

h4pOutput* redLED=new h4pOutput(D6, ACTIVE_HIGH);

void onGPIO(int p, int v){
  if(p==0){
    switch(v){
        case -2:
            Serial.printf("Entering 3nd stage!\n"); // over 5 sec
            sig.flashPin(50,redLED);
            break;
        case -1:
            Serial.printf("Entering 2nd stage!\n"); // 2sec - 5 sec
            sig.flashPin(125,redLED);
            break;
        case 0:
            Serial.printf("SHORT Press\n"); // < 2 sec
            redLED->toggle();
            break;
        case 1:
            Serial.printf("MEDIUM Press\n"); // 2 -5 sec
            sig.stopPin(redLED);
            // wait 0.5 sec then blip it 2x              
            h4.once(500,[=]{ h4.nTimes(2,200,[=]{ sig.pulsePin(50,redLED); }); }); 
            break;
        case 2:
            Serial.printf("LONG Press\n"); // over 5 sec
            sig.stopPin(redLED);
            // wait 0.5 sec then blip it 5x               
            h4.once(500,[=]{ h4.nTimes(5,200,[=]{ sig.pulsePin(50,redLED); }); }); 
            break;
    }
  }
}
//            stage-> 0     1     2 
H4PM_STAGE_MAP breaks{ 2000 , 5000 };

h4pMultistage ms(0,INPUT_PULLUP,ACTIVE_LOW,20,breaks);

void h4pGlobalEventHandler(const std::string& svc,H4PE_TYPE t,const std::string& msg){
    switch(t){
        H4P_FUNCTION_ADAPTER_GPIO;
    }
}
