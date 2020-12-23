#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

H4_FN_UIBOOL randomBool=[]{ return random(0,50) > 25; };

int derivedInt(){ return -1; }
std::string runtimeText(){ return H4Plugin::getConfig("chip").append("/").append(H4P_VERSION); }

H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","uidynamic");
H4P_AsyncWebServer h4asws; //(onViewers,onNoViewers);
H4P_Heartbeat h4hb([]{ h4asws.uiSync(); });

void h4setup(){ 
    h4asws.uiAddLabel("Static Text 2",runtimeText);  
    h4asws.uiAddLabel("Static int 2",derivedInt);  
    h4asws.uiAddLabel("Heap",[](){ return ESP.getFreeHeap(); });  
    h4asws.uiAddLabel("Millis",millis);  
    h4asws.uiAddLabel("Uptime",H4P_Heartbeat::upTime);  
    h4asws.uiAddBoolean("Random Bool",randomBool); 
}
