#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop
//
const char* WIFI_SSID="XXXXXXXX";
const char* WIFI_PASS="XXXXXXXX";
const char* MQTT_SERVER="http://192.168.1.4:1883";
//
const char* USER_TEXT_NAME="User Text";
const char* myText="Select";
const char* usertext="Upper Or Lower?";

H4_TIMER ui,cond;
boolean condition=true;

H4P_PinMachine h4gm;
H4P_WiFi h4wifi(WIFI_SSID,WIFI_PASS,"uiinputs");
H4P_AsyncMQTT h4mqtt(MQTT_SERVER);
H4P_Heartbeat h4hb;
H4P_ConditionalSwitch sw1([](){ return condition; },BUILTIN_LED,H4P_ASSUMED_SENSE,H4P_UILED_GREEN);
H4P_UPNPServer h4upnp("UI Input Tester");

void onViewersConnect(){
  Serial.printf("Ever get that feeling someone is watching you?\n");
  
  h4wifi.uiAddText("go button is...","TACT"); // gets "proper cased"
  h4wifi.uiAddImgButton("Go");
   
  h4wifi.uiAddText("pink button is...","Latching"); // gets "proper cased"
  h4wifi.uiAddImgButton("pink");

  h4wifi.uiAddInput("special");
  h4wifi.uiAddDropdown("Select",{
    {"Both Cases","BcBc"},
    {"Uppercase","UUUU"},
    {"Lowercase","llll"}
  });
  h4wifi.uiAddText(USER_TEXT_NAME,usertext);
  h4wifi.uiAddImg("logo","H4PLogo.jpg");

  ui=h4.everyRandom(60000,90000,[]{ h4wifi.uiMessage("IMPORTANT ANNOUNCEMENT"); });
  cond=h4.everyRandom(15000,30000,[]{
    condition=!condition; 
    sw1.syncCondition();
    h4wifi.uiMessage("CONDITION NOW %sARMED",condition ? "":"DIS");
  });
}

void onViewersDisconnect(){ h4.cancel({ui,cond}); }

void onGoButton(){ h4wifi.uiMessage("Do NOT COLLECT £200!\n"); }

void onpinkButton(bool b){ h4wifi.uiMessage("Wish you were%s here...\n",b ? "":"n't"); }

H4P_EventListener allexceptmsg(H4PE_VIEWERS | H4PE_GVCHANGE,[](const std::string& svc,H4PE_TYPE t,const std::string& msg){
  switch(t){
    case H4PE_VIEWERS:
      H4P_SERVICE_ADAPTER(Viewers);
      break;
    case H4PE_GVCHANGE:
      H4P_TACT_BUTTON_CONNECTOR(Go);
      H4P_LATCH_BUTTON_CONNECTOR(pink);
      if(svc=="Select"){
        h4wifi.uiMessage("You chose %s\n",CSTR(msg));
        if(msg=="llll") h4wifi.uiSetValue(USER_TEXT_NAME,lowercase(usertext)); 
        if(msg=="UUUU") h4wifi.uiSetValue(USER_TEXT_NAME,uppercase(usertext)); 
        if(msg=="BcBc") h4wifi.uiSetValue(USER_TEXT_NAME,usertext); 
        break;
      }
      break;
  }
});

void h4setup(){
  h4p.gvSetInt("Go",0);
  h4p.gvSetInt("pink",0);
  h4p.gvSetstring("special","replay");
}