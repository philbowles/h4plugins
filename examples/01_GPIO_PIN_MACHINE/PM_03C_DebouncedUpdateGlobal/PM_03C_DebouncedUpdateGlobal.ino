#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) 

#define USER_BTN 0
#define UB_ACTIVE ACTIVE_LOW
#define UB_MS 15
// 15mSec of debounce time. This will vary for all hardware: experiment for the best value

h4pDebounced flat(USER_BTN,INPUT_PULLUP,UB_ACTIVE,UB_MS,new npUPDATEGLOBAL{"dbValue"});

H4P_EventListener allexceptmsg(H4PE_VIEWERS | H4PE_GVCHANGE,[](const std::string& svc,H4PE_TYPE t,const std::string& msg){
  switch(t){
    case H4PE_GVCHANGE:
      if(svc=="dbValue") Serial.printf("dbValue now value=%s\n",CSTR(msg));    
      break;
  }
});

void h4setup(){
  h4p.gvSetInt("dbValue",0,false);
}