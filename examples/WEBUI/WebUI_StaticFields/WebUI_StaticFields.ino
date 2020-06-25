
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

boolean boolData(){ return random(0,50) > 25; } // randmly return true / false

// whenever a user browses the UI
void onViewers(){
    // create your own UI fields and "setter" functions
    h4asws.addUILabelText("User Text",[]{ return "some value"; });  
    h4asws.addUIBoolean("User Bool",boolData); // no action function = not clickable  
    h4asws.addUILabelNumeric("Millis",millis);
}

H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","webui");
H4P_AsyncMQTT h4mqtt("192.168.1.4",1883);
H4P_AsyncWebServer h4asws(onViewers);
