#define H4P_VERBOSE 1
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

void h4pGlobalEventHandler(const std::string& svc,H4PE_TYPE t,const std::string& msg){
  switch(t){
    // provides default actions for H4PE_SYSINFO, H4PE_SYSWARN, H4PE_SYSFATAL
    H4P_DEFAULT_SYSTEM_HANDLER
  }   
}

void h4setup(){
  h4.every(1000,[]{ h4psysevent("user",H4PE_SYSINFO,"tickety boo %u\n",millis()); });
  h4.every(5000,[]{ h4psysevent("user",H4PE_SYSWARN,"app will hang after 30seconds!!!!"); });
  h4.once(30000,[]{ h4psysevent("user",H4PE_SYSFATAL,"I DID warn you!!!!"); });
}