#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_FN_UIBOOL randomBool=[]{ return random(0,50) > 25; };
int derivedInt(){ return -1; }
std::string runtimeText(){ return H4Plugin::getConfig("chip").append("/").append(H4P_VERSION); }

H4_TIMER  T1;

void onViewers(){
  Serial.printf("Ever get that feeling someone is watching you?\n");
  T1=h4.every(1000,[](){ h4asws.uiSync(); });
}
// Release global resources when ui is no longer required as all browsers closed
void onNoViewers(){ h4.cancel(T1); }
/*

There is a MUCH easier way of getting the "up Time": see the use of the H4P_Heartbeat plugin
in the WebUI_InputFields example

*/
string upTime(){ 
    char buf[15];
    uint32_t sex=millis()/1000;
    sprintf(buf,"%3d %02d:%02d:%02d",sex/86400,(sex%86400)/3600,(sex/60)%60,sex%60);
    return string(buf);
}

H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","uidynamic");
H4P_AsyncWebServer h4asws(onViewers,onNoViewers);

void h4setup(){ 
    h4asws.uiAddLabel("Static Text 2",runtimeText);  
    h4asws.uiAddLabel("Static int 2",derivedInt);  
    h4asws.uiAddLabel("Heap",[](){ return ESP.getFreeHeap(); });  
    h4asws.uiAddLabel("Millis",millis);  
    h4asws.uiAddLabel("Uptime",upTime);
    h4asws.uiAddBoolean("Random Bool",randomBool); 
}
