#define H4P_VERBOSE 1
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

#define USER_BUTTON 0

boolean boolData(){ return random(0,50) > 25; } // randomly return true / false
int derivedInt(){ return -1; }
std::string runtimeText(){ return string(h4p["chip"]).append("/").append(H4P_VERSION); }

H4_TIMER  TIM0,TIM1;

H4P_SerialLogger woof;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","uistatic");
H4P_BinarySwitch h4onof(D6,ACTIVE_HIGH,OFF);

H4P_EventListener allexceptmsg(H4PE_VIEWERS | H4PE_GV_CHANGE,[](const string& svc,H4PE_TYPE t,const string& msg){
  switch(t){
    case H4PE_VIEWERS:
      if(atoi(msg.c_str())){
       Serial.printf("Ever get that feeling someone is watching you?\n");
       TIM0=h4.everyRandom(2500,5500,[](){ h4wifi.uiSetBoolean("Random Bool",boolData()); });
       TIM1=h4.every(1000,[](){
          h4wifi.uiSetLabel("Heap",ESP.getFreeHeap());  
          h4wifi.uiSetLabel("Millis",millis());
        });      
      } else h4.cancel({TIM0,TIM1}); 
      break;
  }
});

void h4setup(){
    Serial.printf("Adding WebUI User fields\n");
    h4p.gvSetInt("True Bool",1,false);
    h4p.gvSetInt("False Bool",0,false);
    h4p.gvSetInt("Random Bool",boolData(),false);
    h4wifi.uiAddLabel("static text 1","FIXED"); // gets "proper cased"
    h4p.gvSetInt("auto",666,false);
    h4wifi.uiAddLabel("Static Text 2",runtimeText()); 
    h4wifi.uiAddLabel("Static int 1",42);  
    h4wifi.uiAddLabel("Static int 2",derivedInt());  
    h4wifi.uiAddBoolean("True Bool");  
    h4wifi.uiAddBoolean("False Bool");
    
    h4wifi.uiAddLabel("Heap",ESP.getFreeHeap());  
    h4wifi.uiAddLabel("Millis",millis());  
    h4wifi.uiAddBoolean("Random Bool");
}