#include<H4Plugins.h>
H4_USE_PLUGINS(0,20,true) // Serial baud rate, Q size, SerialCmd autostop

#define LIGHT D1
#define ARMED D2
#define SQUAWK D6

boolean armed=false;

uint32_t arm(vector<string> vs); // necessary fwd ref

void onMQTTConnect(){ h4mqtt.subscribeDevice("arm",arm); }

void onMQTTDisconnect(){ h4mqtt.unsubscribeDevice("arm"); }

//H4P_SerialLogger h4sl;
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
bool predicate(bool b){ return armed; }

H4P_ConditionalSwitch h4onof(SQUAWK,ACTIVE_HIGH,OFF,predicate);

H4P_UPNPServer h4upnp("Squawkbox");

void h4setup(){
//  pSquawk=h4gm.Output(SQUAWK,ACTIVE_HIGH,OFF);  
    h4gm.Polled(LIGHT,INPUT,ACTIVE_HIGH,60000,false,[](H4GPIOPin* ptr){
        H4GM_PIN(Polled); // Create the correct pointer type in 'pin'
        arm({stringFromInt(pin->state)});
    });
}

void armingStateChange(bool b){
  if(b) h4fc.flashLED(500,ARMED);
  else {
    h4fc.stopLED(ARMED);
    h4onof.turn(OFF); 
  }
  h4mqtt.publishDevice("armed",armed=b);
  h4onof.syncCondition();
}

uint32_t arm(vector<string> vs){
  if(vs.size()){
    bool b=H4PAYLOAD_INT;
    if(b!=armed) armingStateChange(b); 
    else Serial.printf("Nothing to do!\n");  
    return H4_CMD_OK;
  }
  else return H4_CMD_TOO_FEW_PARAMS;
}
