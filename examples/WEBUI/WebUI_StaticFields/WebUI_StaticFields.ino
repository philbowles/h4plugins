#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

boolean boolData(){ return random(0,50) > 25; } // randomly return true / false

H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","webui");
H4P_AsyncMQTT h4mqtt("192.168.1.4",1883);
H4P_AsyncWebServer h4asws;

void h4setup(){
    h4asws.uiAddLabelText("User Text",[]{ return "some value"; });  
    h4asws.uiAddBoolean("User Bool",boolData); // no action function = not clickable  
    h4asws.uiAddLabelNumeric("Millis",millis); 
}
