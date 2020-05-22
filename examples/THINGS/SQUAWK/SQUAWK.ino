#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

#define LIGHT D1
#define ARMED D2
#define SQUAWK D6

boolean armed=false;

uint32_t arm(vector<string> vs){
  if(armed=H4PAYLOAD_INT) h4fc.flashLED(500,ARMED);
  else h4fc.stopLED(ARMED);
  h4mqtt.publishDevice("armed",H4PAYLOAD_INT);
  return H4_CMD_OK; 
}

void onMQTTConnect(){ h4mqtt.subscribeDevice("arm",arm); }

void onMQTTDisconnect(){ h4mqtt.unsubscribeDevice("arm"); }

H4P_GPIOManager h4gm;
H4P_FlasherController h4fc;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","squawk");
H4P_AsyncMQTT h4mqtt("192.168.1.4",1883,"","",onMQTTConnect,onMQTTDisconnect);
H4P_AsyncWebServer h4asws;
/*
OutputPin* pSquawk;

H4P_BinaryThing h4onof([](bool b){
    Serial.printf("SQUAWK %d [armed=%d]\n",b,armed);
    if(pSquawk) pSquawk->logicalWrite(b & armed);
  });

H4P_ConditionalThing h4onof([](bool b){
    Serial.printf("SQUAWK %d [armed=%d]\n",b,armed);
    if(pSquawk) pSquawk->logicalWrite(b);
  },
  [](bool b){ return armed; }
);
 */ 
H4P_ConditionalSwitch h4onof(SQUAWK,ACTIVE_HIGH,OFF,[](bool b){ return armed; });
 
H4P_UPNPServer h4upnp("Squawkbox");

void h4setup(){
//  pSquawk=h4gm.Output(SQUAWK,ACTIVE_HIGH,OFF);  
    h4gm.Polled(LIGHT,INPUT,ACTIVE_HIGH,60000,false,[](H4GPIOPin* ptr){
        H4GM_PIN(Polled); // Create the correct pointer type in 'pin'
        arm({stringFromInt(pin->state)});
    });
}
