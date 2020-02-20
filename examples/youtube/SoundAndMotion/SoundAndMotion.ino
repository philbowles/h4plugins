#include<H4Plugins.h>
H4_USE_PLUGINS

#define LED_RED     D6
#define LED_YELLOW  D7
#define LED_GREEN   D8

#define MOTION      D1
#define LIGHT       A0

#define F_POLL      5000

H4 h4(115200);
H4P_SerialCmd h4sc;
H4P_GPIOManager h4gm;
//H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","h4plugins");
//H4P_MQTT h4mqtt("192.168.1.4",1883);
//H4P_AsyncWebServer h4asws("admin","admin");
//H4P_UPNPSwitch h4upnp("Demo Switch",RELAY_BUILTIN,ACTIVE_HIGH,OFF);
H4P_BinarySwitch h4onof(LED_RED,ACTIVE_HIGH,OFF);
H4P_FlasherController h4fc;
H4P_ThreeFunctionButton h43fb(&h4onof,15,BUTTON_BUILTIN,INPUT,ACTIVE_LOW,LED_BUILTIN,ACTIVE_LOW);

void h4setup(){
    h4gm.RetriggeringThing(MOTION,INPUT,ACTIVE_HIGH,10000,&h4onof); // 10sec motion timeout
    h4gm.Polled(LIGHT,INPUT,ACTIVE_HIGH,F_POLL,true,[](H4GPIOPin* ptr){
        H4GM_PIN(Polled);
        Serial.printf("POLLED = %d\n",pin->state);
        h4onof.turn(pin->state < 60 ? 1:0);
    }); // door alarm
}
