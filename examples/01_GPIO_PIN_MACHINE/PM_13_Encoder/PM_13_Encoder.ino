#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) 

#define USER_BTN 0
#define UB_ACTIVE ACTIVE_LOW
#define UB_MS   15

h4pOutput builtinLED(LED_BUILTIN, ACTIVE_LOW,OFF);

H4P_EventListener gpio(H4PE_GPIO,[](const string& pin,H4PE_TYPE t,const string& msg){
    int p=atoi(pin.c_str());
    int v=atoi(msg.c_str());
    switch(p){
      case USER_BTN:
        Serial.printf("P%d V = %d\n",p,v);
        switch(v){
            case -2:
                Serial.printf("Entering 3nd stage!\n"); // over 5 sec
                break;
            case -1:
                Serial.printf("Entering 2nd stage!\n"); // 2sec - 5 sec
                break;
            case 0:
                Serial.printf("SHORT Press\n"); // < 2 sec
                builtinLED.toggle();
                break;
            case 1:
                Serial.printf("MEDIUM Press\n"); // 2 -5 sec
                builtinLED.toggle();
                builtinLED.toggle();
                builtinLED.toggle();
                break;
            case 2:
                Serial.printf("LONG Press\n"); // over 5 sec
                builtinLED.toggle();
                builtinLED.toggle();
                builtinLED.toggle();
                builtinLED.toggle();
                builtinLED.toggle();
                break;
        }
        break;
    }
});

H4PM_STAGE_MAP breaks{2000,5000};

h4pMultistage ms(USER_BTN,INPUT,ACTIVE_HIGH,UB_MS,breaks);
