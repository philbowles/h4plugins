#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop
//
//    Configuration
//
const char* WIFI_SSID="XXXXXXXX";
const char* WIFI_PASS="XXXXXXXX";
const char* MQTT_SERVER="192.168.1.4";
const int   MQTT_PORT=1883;
const char* REMOTE_UPDATE_URL="192.168.1.4:1880/update";
//
const char* USER_TEXT_NAME="User Text";
const char* usertext="Upper Or Lower?";

boolean condition=true;

H4P_FN_UIBOOL randomBool=[]{ return random(0,50) > 25; };

void userBoolAction(const string& name,const string& b){ h4asws.uiMessage("%s was clicked! %s\n",CSTR(name),CSTR(b)); }

H4_TIMER msg,cond;

void onViewers(); // necessary fwd reference
void onNoViewers(){ h4.cancel({msg,cond}); }

void onMQTTError(uint8_t e,int info){ Serial.printf("USER MQTT ERROR %d INFO %d\n",e,info); }

H4P_SerialLogger h4sl;
H4P_GPIOManager h4gm;
H4P_WiFi h4wifi(WIFI_SSID,WIFI_PASS,"uiinputs");
H4P_AsyncMQTT h4mqtt(MQTT_SERVER,MQTT_PORT);
H4P_AsyncWebServer h4asws(onViewers,onNoViewers);
//H4P_Heartbeat h4hb([]{ Serial.printf("PING %u\n",millis()); });
H4P_Heartbeat h4hb;
H4P_ConditionalSwitch h4onof(LED_BUILTIN,ACTIVE_LOW,OFF,[](bool b){ return condition; },nullptr,2000); // AutoOff=2sec

void onViewers(){
    msg=h4.everyRandom(60000,90000,[]{ h4asws.uiMessage("IMPORTANT ANNOUNCEMENT"); });
    cond=h4.everyRandom(15000,30000,[]{
      condition=!condition; 
      h4onof.syncCondition();
      h4asws.uiMessage("CONDITION NOW %sARMED",condition ? "":"DIS");
    });
}

void onUiChange(const string& name,const string& value){ 
  if(name=="autoOff"){
    Serial.printf("USER autoOff changed to %s\n",CSTR(value));
    h4onof.autoOff(atoi(CSTR(value)));    
  } 
  else {
    if(name=="Select"){
      h4asws.uiMessage("You chose %s\n",CSTR(value));
      if(value=="llll") h4asws.uiSetLabel(USER_TEXT_NAME,lowercase(usertext)); 
      if(value=="UUUU") h4asws.uiSetLabel(USER_TEXT_NAME,uppercase(usertext)); 
      if(value=="BcBc") h4asws.uiSetLabel(USER_TEXT_NAME,usertext); 
    }
    else Serial.printf("USER onUiChange %s no action defined v=%s\n",CSTR(name),CSTR(value));
  }
}

void h4setup(){
//  create your own UI fields and "setter" functions
    H4Plugin::setConfig("autoOff",h4onof.getAutoOff()); // create config item to hold changing autoOff value
    h4asws.uiAddInput("autoOff");  // make it an input field in the ui
    h4asws.uiAddDropdown("Select",{
      {"Both Cases","BcBc"},
      {"Uppercase","UUUU"},
      {"Lowercase","llll"}
    });
    h4asws.uiAddLabel(USER_TEXT_NAME,usertext);
    h4asws.uiAddBoolean("Clicky Bool",randomBool,userBoolAction);  
}
