#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

boolean boolData(){ return random(0,50) > 25; }

H4_TIMER  millisTimer;

// whenever a user browses the UI
void onViewers(){
    // create your own UI fields and "setter" functions
    h4asws.addUILabelText("User",[]{ return "Has own fields"; });  
    h4asws.addUILabelText("My Text",[]{ return "some value"; });  
    h4asws.addUIBoolean("My Bool",boolData);  
    h4asws.addUILabelNumeric("Millis",millis);
    // setup conditions for updating fields (if required)
    h4.once(60000,[]{ h4asws.setUILabelText("My Text",[]{ return "some other data"; }); });  
    h4.everyRandom(15000,20000,[]{ h4asws.setUIBoolean("My Bool",boolData); });
    h4.everyRandom(25000,30000,[]{ h4asws.sendUIMessage("IMPORTANT ANNOUNCEMENT"); });
    millisTimer=h4.every(1000,[]{ h4asws.setUILabelNumeric("Millis",millis); }); 
}

// when ui is no longer required as all browsers closed
void onNoViewers(){  h4.cancel(millisTimer); }

H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","webui");
H4P_AsyncMQTT h4mqtt("192.168.1.4",1883);
H4P_AsyncWebServer h4asws(onViewers,onNoViewers);
