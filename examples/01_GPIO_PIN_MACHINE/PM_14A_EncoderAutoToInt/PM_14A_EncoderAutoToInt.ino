#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) 

#define USER_BTN 0
#define UB_ACTIVE ACTIVE_LOW
#define UB_MS 15
// Range -10 - +10 in increments of 1, start position 50% wrap aroun at ends
h4pEncoderAuto rotary(D1,D2,INPUT,ACTIVE_HIGH,-10,10,1,50,true,new npUPDATEGLOBAL{"eaValue"});
h4pDebounced centerbutton(USER_BTN,INPUT,UB_ACTIVE,UB_MS); // 15 ms debounce time

H4P_EventListener gpio(H4PE_GPIO | H4PE_GVCHANGE,[](const std::string& svc,H4PE_TYPE t,const std::string& msg){
    switch(t){
        case H4PE_GPIO:
          {
              int p=atoi(svc.c_str());
              int v=atoi(msg.c_str());
              switch(p){
                  case USER_BTN:
                      rotary.center();
                      Serial.printf("CENTERED: value is %d\n",rotary.getValue());
                      break;
                  }
          }
          break;
        case H4PE_GVCHANGE:
          if(svc=="eaValue") Serial.printf("eaValue now=%s\n",CSTR(msg));    
          break;
    }
});

void h4setup(){ h4p.gvSetInt("eaValue",0,false); }