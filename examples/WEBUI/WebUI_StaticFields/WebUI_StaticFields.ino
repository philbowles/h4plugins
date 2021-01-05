#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

#define USER_BUTTON 0

boolean boolData(){ return random(0,50) > 25; } // randomly return true / false
int derivedInt(){ return -1; }
std::string runtimeText(){ return H4Plugin::getConfig("chip").append("/").append(H4P_VERSION); }

H4_TIMER  T1;

void onViewers(){
  Serial.printf("Ever get that feeling someone is watching you?\n");
  T1=h4.every(1000,[](){
      h4asws.uiSetBoolean("Random Bool",boolData());
      h4asws.uiSetLabel("Heap",ESP.getFreeHeap());  
      h4asws.uiSetLabel("Millis",millis());
    });
}
// Release global resources when ui is no longer required as all browsers closed
void onNoViewers(){ h4.cancel(T1); }

H4P_GPIOManager h4gm;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","uistatic");
H4P_AsyncWebServer h4asws(onViewers,onNoViewers);
H4P_BinarySwitch h4onof(RELAY_BUILTIN,ACTIVE_HIGH,OFF);

void h4setup(){
    h4gm.Raw(USER_BUTTON,INPUT,ACTIVE_LOW,[](H4GPIOPin* ptr){});
    Serial.printf("Adding WebUI User fields\n");
    h4asws.uiAddLabel("static text 1","FIXED"); // gets "proper cased"
    h4asws.uiAddLabel("auto"); // if no initial value given, defaults to config Item of same name  
    h4asws.uiAddLabel("Static Text 2",runtimeText()); 
    h4asws.uiAddLabel("Static int 1",42);  
    h4asws.uiAddLabel("Static int 2",derivedInt());  
    h4asws.uiAddLabel("Heap",ESP.getFreeHeap());  
    h4asws.uiAddLabel("Millis",millis());  

    h4asws.uiAddBoolean("True Bool",true);  
    h4asws.uiAddBoolean("False Bool",false);
    h4asws.uiAddBoolean("Random Bool",boolData());

    h4asws.uiAddGPIO(USER_BUTTON); 
}