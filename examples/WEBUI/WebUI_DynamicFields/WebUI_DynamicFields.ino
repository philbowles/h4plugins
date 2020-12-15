#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

boolean condition=true;

boolean boolData(){ return random(0,50) > 25; }

void userBoolAction(const string& name,const string& b){ h4asws.uiMessage("%s was clicked! %s\n",CSTR(name),CSTR(b)); }

H4_TIMER ub,msg,txt,cond;

void onViewers(); // necessary fwd reference
// Release global resources when ui is no longer required as all browsers closed
void onNoViewers(){ h4.cancel({ub,msg,txt,cond}); }

void onMQTTError(uint8_t e,int info){ Serial.printf("USER MQTT ERROR %d INFO %d\n",e,info); }

H4P_GPIOManager h4gm;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","webui");
H4P_AsyncMQTT h4mqtt("192.168.1.21",1883,"example","pangolin");
H4P_AsyncWebServer h4asws(onViewers,onNoViewers);
H4P_Heartbeat h4hb([]{ h4asws.uiSync("Uptime"); });
H4P_ConditionalSwitch h4onof(LED_BUILTIN,ACTIVE_LOW,OFF,[](bool b){ return condition; });
H4P_UPNPServer h4upnp("V0.5.9 Active UI Tester");

void onViewers(){
    // setup conditions for updating fields (if required)
    ub=h4.everyRandom(15000,20000,[]{ // Normal method is to sync: calls the "setter" function provided to uiAddxxx
      h4asws.uiSync("User Bool");
      h4asws.uiSync("User Number");
    });
    txt=h4.onceRandom(17000,25000,[]{ h4asws.uiSetLabelText("User Text",H4P_VERSION); }); // don't call uiAddxxx "setter", use arbitrary value
    msg=h4.everyRandom(25000,30000,[]{ h4asws.uiMessage("IMPORTANT ANNOUNCEMENT"); });
    cond=h4.everyRandom(15000,30000,[]{
      condition=!condition; 
      h4onof.syncCondition();
      h4asws.uiMessage("CONDITION NOW %sARMED",condition ? "":"DIS");
    });
}

void h4setup(){
//  create your own UI fields and "setter" functions
    h4asws.uiAddLabelNumeric("User Number",[]{ return random(42,666); });  
    h4asws.uiAddLabelText("User Text",[]{ return "INITIAL"; }); 
    h4asws.uiAddBoolean("User Bool",boolData); // no action: function = not clickable  
    h4asws.uiAddBoolean("User Bool 1",boolData,userBoolAction);  
    h4asws.uiAddBoolean("User Bool 2",boolData,userBoolAction);  
    h4asws.uiAddLabelText("Uptime",H4P_Heartbeat::upTime);
    h4asws.uiAddGPIO(LED_BUILTIN);  
}