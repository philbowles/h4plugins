#include<H4Plugins.h>
H4_USE_PLUGINS(115200,10,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_FN_UIBOOL randomBool=[]{ return random(0,50) > 25; };
int derivedInt(){ return -1; }
std::string runtimeText(){ return H4Plugin::getConfig("chip").append("/").append(H4P_VERSION); }

H4_TIMER  Tim1;


// Release global resources when ui is no longer required as all browsers closed
void onNoViewers(){ h4.cancel(Tim1); }
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

void onViewers(){
  Serial.printf("Ever get that feeling someone is watching you?\n");
  Tim1=h4.every(1000,[](){ h4wifi.uiSync(); });
}

void h4setup(){ 
    h4wifi.uiAddLabel("Static Text 2",runtimeText);  
    h4wifi.uiAddLabel("Static int 2",derivedInt);  
    h4wifi.uiAddLabel("Heap",[](){ return ESP.getFreeHeap(); },true);  
    h4wifi.uiAddLabel("Millis",millis,true);  
    h4wifi.uiAddLabel("Uptime",upTime,true);
    h4wifi.uiAddBoolean("Random Bool",randomBool,nullptr,true); 
}