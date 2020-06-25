
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

boolean boolData(){ return random(0,50) > 25; }

void userBoolAction(const string& name,const string& b){ 
  h4asws.sendUIMessage("%s was clicked! %s\n",CSTR(name),CSTR(b));
}

H4_TIMER  millisTimer;

// whenever a user browses the UI
void onViewers(){
    // create your own UI fields and "setter" functions
    h4asws.addUILabelText("User Text",[]{ return "some value"; });  
    h4asws.addUIBoolean("User Bool",boolData); // no action function = not clickable  
    h4asws.addUIBoolean("User Bool 1",boolData,userBoolAction);  
    h4asws.addUIBoolean("User Bool 2",boolData,userBoolAction);  
    h4asws.addUILabelNumeric("Millis",millis);
    // setup conditions for updating fields (if required)
    h4.everyRandom(15000,20000,[]{ h4asws.setUIBoolean("User Bool",boolData); });
    h4.everyRandom(15000,20000,[]{ h4asws.setUIBoolean("User Bool 1",boolData); });
    h4.everyRandom(15000,20000,[]{ h4asws.setUIBoolean("User Bool 2",boolData); });
    h4.everyRandom(25000,30000,[]{ h4asws.sendUIMessage("IMPORTANT ANNOUNCEMENT"); });
    millisTimer=h4.every(1000,[]{ h4asws.setUILabelNumeric("Millis",millis); }); 
}

// when ui is no longer required as all browsers closed
void onNoViewers(){  h4.cancel(millisTimer); }

H4P_GPIOManager h4gm;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","webui");
H4P_AsyncMQTT h4mqtt("192.168.1.4",1883);
H4P_AsyncWebServer h4asws(onViewers,onNoViewers);

H4P_ConditionalSwitch h4onof(RELAY_BUILTIN,ACTIVE_HIGH,OFF,[](bool b){ return true; });
H4P_UPNPServer h4upnp("V0.5.6 Active UI Tester");