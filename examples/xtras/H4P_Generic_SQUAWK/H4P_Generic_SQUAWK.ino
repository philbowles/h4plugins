#define H4P_VERBOSE 1
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop
//H4_USE_PLUGINS(0,20,true) // Serial baud rate, Q size, SerialCmd autostop
//
//  This is designed for an external alarm box, which will only "arm" itself after dark
//  (it can be manually overridden and armed via MQTT, command line etc)
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
const char* MQTT_SERVER="http://192.168.1.4:1883";
const char* MQTT_USER="";
const char* MQTT_PASS="";
//const char* REMOTE_UPDATE_URL="http://192.168.1.4:1880/update";

boolean armed=false;

H4P_SerialLogger slog;
H4P_PinMachine h4gm;
H4P_Signaller h4fc;
H4P_WiFi h4wifi(WIFI_SSID,WIFI_PASS);
H4P_AsyncMQTT mqtt(MQTT_SERVER,MQTT_USER,MQTT_PASS);
H4P_LinkMaster  lynx;
H4P_ConditionalSwitch h4onof([](){ return armed; },SQUAWK,ACTIVE_HIGH);
H4P_UPNPServer h4upnp;
H4P_RemoteUpdate h4ru;

h4pPolled ark(LIGHT,INPUT,ACTIVE_HIGH,5000);
h4pOutput alert(ARMED,ACTIVE_HIGH,H4P_UILED_RED);

uint32_t arm(std::vector<std::string> vs){
    if(vs.size()){
        bool b=H4PAYLOAD_INT;
        if(b!=armed) armingStateChange(b); 
        return H4_CMD_OK;
    } else return H4_CMD_TOO_FEW_PARAMS;
}

void onMqttConnect(){ mqtt.subscribeDevice("arm",arm);}
void onMqttDisconnect(){}

void armingStateChange(bool b){
    if(b) h4fc.flashPin(500,alert);
    else {
        h4fc.stopPin(alert);
        h4onof.turn(OFF); 
    }
    mqtt.publishDevice("armed",armed=b);
    h4onof.syncCondition();
}

void onGPIO(int pin,int value){ if(pin==LIGHT) armingStateChange(value); }

void h4pGlobalEventHandler(const std::string& svc,H4PE_TYPE t,const std::string& msg){
    switch(t){
//        H4P_DEFAULT_SYSTEM_HANDLER
        H4P_FUNCTION_ADAPTER_GPIO;
        case H4PE_SERVICE:
            H4P_SERVICE_ADAPTER(Mqtt);
            break;
    }
}