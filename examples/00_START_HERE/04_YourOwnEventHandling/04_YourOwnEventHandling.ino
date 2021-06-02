#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

void h4pGlobalEventHandler(const std::string& svc,H4PE_TYPE t,const std::string& msg){
  switch(t){
    // provides default actions for H4PE_SYSINFO, H4PE_SYSWARN, H4PE_SYSFATAL
    H4P_DEFAULT_SYSTEM_HANDLER
  }   
}
// event listeners
H4P_EventListener allexceptmsg(H4PE_ALMOST_ALL &~ H4PE_MSG,[](const std::string& svc,H4PE_TYPE t,const std::string& msg){ 
  Serial.printf("Usr2 sez %s %s\n",CSTR(h4pGetEventName(t)),CSTR(msg)); 
});
H4P_EventListener hb(H4PE_HEARTBEAT,[](const std::string& svc,H4PE_TYPE t,const std::string& msg){ 
  Serial.printf("User can hear your heartbeat %s\n",CSTR(msg)); 
});

// event emitters
H4P_EmitTick tick; // 1x per second sends H4PE_HEARTBEAT
H4P_EmitHeap heep(5000); // every 5 seconds sends H4PE_HEAP
H4P_EmitQ eq(2000); // H4PE_Q every 2secs

void h4setup(){
  h4.everyRandom(5000,10000,[](){ h4pUserEvent("wow signal @ %u",millis()); }); // event emitter
  h4pOnEvent(H4PE_MSG,[](const std::string& msg){ Serial.printf("User message event occurred %s\n",CSTR(msg)); });
}