#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop
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

H4_TIMER ui,cond;
boolean condition=true;
uint32_t myText;

H4P_FN_UIBOOL randomBool=[]{ return random(0,50) > 25; };

// necessary fwd reference
void clickyBoolAction(const string& b);
 
H4P_SerialLogger h4sl;
H4P_PinMachine h4gm;
H4P_WiFi h4wifi(WIFI_SSID,WIFI_PASS,"uiinputs");
H4P_AsyncMQTT h4mqtt(MQTT_SERVER,MQTT_PORT);
H4P_Heartbeat h4hb;

H4P_ConditionalSwitch sw1([](){ return condition; },BUILTIN_LED,H4P_ASSUMED_SENSE,H4P_UILED_GREEN);
H4P_UPNPServer h4upnp("UI Input Tester");

H4P_EventListener allexceptmsg(H4PE_VIEWERS | H4PE_GVCHANGE,[](const string& svc,H4PE_TYPE t,const string& msg){
  switch(t){
    case H4PE_VIEWERS:
      if(STOI(msg)){
        ui=h4.everyRandom(60000,90000,[]{ h4wifi.uiMessage("IMPORTANT ANNOUNCEMENT"); });
        cond=h4.everyRandom(15000,30000,[]{
          condition=!condition; 
          sw1.syncCondition();
          h4wifi.uiMessage("CONDITION NOW %sARMED",condition ? "":"DIS");
        });
      }
      else h4.cancel({ui,cond}); 
      break;
    case H4PE_GVCHANGE:
      if(svc=="Select"){
        h4wifi.uiMessage("You chose %s\n",CSTR(msg));
        if(msg=="llll") h4wifi.uiSetLabel(myText,lowercase(usertext)); 
        if(msg=="UUUU") h4wifi.uiSetLabel(myText,uppercase(usertext)); 
        if(msg=="BcBc") h4wifi.uiSetLabel(myText,usertext); 
        break;
      }
      if(svc=="Clicky Bool"){
        h4wifi.uiMessage("ClickyBool was clicked! %s\n",CSTR(msg));
        break;
      }
      Serial.printf("USER PS CHANGE %s no action defined v=%s\n",CSTR(svc),CSTR(msg));    
      break;
  }
});

void h4setup(){
//  create your own UI fields and "setter" functions
    h4wifi.uiAddInput("special","replay");
    h4wifi.uiAddDropdown("Select",{
      {"Both Cases","BcBc"},
      {"Uppercase","UUUU"},
      {"Lowercase","llll"}
    });
    myText=h4wifi.uiAddLabel(USER_TEXT_NAME,usertext);
    h4wifi.uiAddBoolean("Clicky Bool",randomBool,true,true);  
}