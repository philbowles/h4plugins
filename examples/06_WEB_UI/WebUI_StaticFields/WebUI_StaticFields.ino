#define H4P_VERBOSE 1
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

boolean condition=false;
boolean boolData(){ return random(0,50) > 25; } // randomly return true / false

H4_TIMER  TIM0,TIM1;

H4P_SerialLogger woof(H4PE_ALMOST_ALL &~ H4PE_GPIO);
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","uistatic");
H4P_AsyncMQTT kewpie("robot.local",1883);
//H4P_BinarySwitch h4onof(D6,ACTIVE_HIGH,OFF);
H4P_ConditionalSwitch h4onof([]{ return condition; },D6,ACTIVE_HIGH,OFF);

void onViewersConnect(){
  Serial.printf("Ever get that feeling someone is watching you?\n");
  
  h4wifi.uiAddText("go button is...","TACT"); // gets "proper cased"
  h4wifi.uiAddText("pink button is...","Latching"); // gets "proper cased"
    
  h4puiAdd("Go",H4P_UI_IMGBTN);
  h4puiAdd("pink",H4P_UI_IMGBTN);
  
  h4wifi.uiAddGlobal("global"); 
  h4wifi.uiAddBoolean("True Bool");  
  h4wifi.uiAddBoolean("False Bool");
  h4wifi.uiAddBoolean("Random Bool");
  
  h4wifi.uiAddText("Millis",millis());  

  TIM0=h4.everyRandom(5000,10000,[](){
    h4wifi.uiSetBoolean("Random Bool",boolData());
    condition=random(0,100) > 50;
    h4pUIMessage("Sync condition now %d",condition); 
    h4onof.syncCondition();
  });
  TIM1=h4.every(1000,[](){ h4wifi.uiSetValue("Millis",millis()); });       
}

void onViewersDisconnect(){ h4.cancel({TIM0,TIM1}); }

void onGoButton(){ Serial.printf("Do NOT COLLECT £200!\n"); }
void onpinkButton(bool b){ Serial.printf("Wish you were%s here...\n",b ? "":"n't"); }

H4P_EventListener allexceptmsg(H4PE_VIEWERS | H4PE_GVCHANGE,[](const string& svc,H4PE_TYPE t,const string& msg){
  switch(t){
    case H4PE_VIEWERS:
      H4P_SERVICE_ADAPTER(Viewers);
      break;
    case H4PE_GVCHANGE:
      Serial.printf("GLOBAL VARIABLE %s now= %s\n",CSTR(svc),CSTR(msg));
      H4P_TACT_BUTTON_HANDLER(Go);
      H4P_LATCH_BUTTON_HANDLER(pink);
      break;
  }
});

void h4setup(){
  // creat globals that will be mapped to the UI
  h4p.gvSetInt("Go",0);
  h4p.gvSetInt("pink",0);
  h4p.gvSetInt("True Bool",1);
  h4p.gvSetInt("False Bool",0);
  h4p.gvSetInt("Random Bool",boolData());
  h4p.gvSetstring("global","variable");
}