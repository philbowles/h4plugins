#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","webuinclusions");
H4P_AsyncHTTP ah;

void h4pGlobalEventHandler(const std::string& svc,H4PE_TYPE t,const std::string& msg){
  switch(t){
    case H4PE_SERVICE:
      if(svc=="http" && (!h4p.gvExists("country"))) {
        ah.GET("http://ip-api.com/json/",[](ARMA_HTTP_REPLY r){ ah.globalsFromSimpleJson(r); }); 
      }
      break;
    case H4PE_VIEWERS:
      if(atoi(msg.data())) h4wifi.uiAddAllUsrFields(); 
      break;
  }
}