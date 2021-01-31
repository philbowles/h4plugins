#include<H4Plugins.h>
H4_USE_PLUGINS(0,20,true) // Serial baud rate, Q size, SerialCmd autostop
//
//  This is designed for an external alarm box, which will only "arm" itself after dark
//  (it can be manually overridden nd armed via MQTT, command line etc)
//  Hardware pins:
//      D1 Light Sensor, D2 "Armed" flashing LED, D3 Siren
//
#define LIGHT D1
#define ARMED D2
#define SQUAWK D6
//
//    Configuration
//
const char* WIFI_SSID="XXXXXXXX";
const char* WIFI_PASS="XXXXXXXX";
const char* MQTT_SERVER="192.168.1.4";
const int   MQTT_PORT=1883;
const char* MQTT_USER="";
const char* MQTT_PASS="";
const char* REMOTE_UPDATE_URL="192.168.1.4:1880/update";

boolean armed=false;

// necessary forward references
void onMQTTConnect();
void onMQTTDisconnect();

H4P_GPIOManager h4gm;
H4P_FlasherController h4fc;
H4P_WiFi h4wifi(WIFI_SSID,WIFI_PASS);
H4P_AsyncMQTT h4mqtt(MQTT_SERVER,MQTT_PORT,MQTT_USER,MQTT_PASS,onMQTTConnect,onMQTTDisconnect);
H4P_ConditionalSwitch h4onof(SQUAWK,ACTIVE_HIGH,OFF,[](bool){ return armed; });
H4P_UPNPServer h4upnp;
H4P_RemoteUpdate h4ru(REMOTE_UPDATE_URL,__FILE__);

void onMQTTConnect(){ h4mqtt.subscribeDevice("arm",arm); }
void onMQTTDisconnect(){ h4mqtt.unsubscribeDevice("arm"); }

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
    return H4_CMD_OK;
  } else return H4_CMD_TOO_FEW_PARAMS;
}

void h4setup(){
    h4gm.Polled(LIGHT,INPUT,ACTIVE_HIGH,60000,false,[](H4GPIOPin* ptr){
        H4GM_PIN(Polled); // Create the correct pointer type in 'pin'
        arm({stringFromInt(pin->state)});
    });
}