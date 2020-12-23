#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,true) // Serial baud rate, Q size, SerialCmd autostop

boolean condition=true;

H4_FN_UIBOOL randomBool=[]{ return random(0,50) > 25; };

void userBoolAction(const string& name,const string& b){ h4asws.uiMessage("%s was clicked! %s\n",CSTR(name),CSTR(b)); }

H4_TIMER msg,cond;

void onViewers(); // necessary fwd reference
void onNoViewers(){ h4.cancel({msg,cond}); }

void onMQTTError(uint8_t e,int info){ Serial.printf("USER MQTT ERROR %d INFO %d\n",e,info); }

H4P_GPIOManager h4gm;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","uiInput");
H4P_AsyncMQTT h4mqtt("192.168.1.21",1883,"example","pangolin");
H4P_AsyncWebServer h4asws(onViewers,onNoViewers);
H4P_Heartbeat h4hb([]{ h4asws.uiSync(); });
H4P_ConditionalSwitch h4onof(LED_BUILTIN,ACTIVE_LOW,OFF,[](bool b){ return condition; },nullptr,2000); // AutoOff=2sec
H4P_UPNPServer h4upnp("V0.5.9 Active UI Tester");

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
    if(name=="Select") h4asws.uiMessage("You chose %s\n",CSTR(value));
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
    h4asws.uiAddBoolean("Clicky Bool",randomBool,userBoolAction);  
    h4asws.uiAddLabel("Uptime",H4P_Heartbeat::upTime);
}