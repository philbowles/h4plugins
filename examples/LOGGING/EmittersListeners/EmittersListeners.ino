#include<H4Plugins.h>
H4_USE_PLUGINS(115200,10,false) // Serial baud rate, Q size, SerialCmd autostop

void h4pGlobalEventHandler(H4PID pid,H4P_EVENT_TYPE t,const string& msg){
  Serial.printf("Big Brother hears EVERYTHING type=%s src=%s %s\n",CSTR(h4pgetEventName(t)),CSTR(h4pnames[pid]),CSTR(msg));
}

H4P_VerboseMessages h4vm;
// event emitters
H4P_EmitTick tick; // 1x per second sends H4P_EVENT_HEARTBEAT
H4P_EmitHeap heep(5000); // every 5 seconds sends H4P_EVENT_HEAP
H4P_EmitQ eq(2000); // H4P_EVENT_Q every 2secs

// event listeners 
// All loggers are event listeners, they simply "foward" the messages somewhere useful
H4P_SerialLogger h4sl(H4P_EVENT_HEAP | H4P_EVENT_Q); // only react to heap and Q

H4P_EventListener hb(H4P_EVENT_HEARTBEAT,[](H4PID pid,H4P_EVENT_TYPE t,const string& msg){ Serial.printf("User can hear your heartbeat %s\n",CSTR(msg)); });
H4P_EventListener allexceptuser(H4P_EVENT_ALL &~ H4P_EVENT_USER,[](H4PID pid,H4P_EVENT_TYPE t,const string& msg){ Serial.printf("Usr2 sez %s\n",CSTR(msg)); });

void h4setup(){
  h4.everyRandom(5000,10000,[](){ h4UserEvent("wow signal @ %u",millis()); }); // event emitter
  h4pOnEvent(H4P_EVENT_USER,[](const string& msg){ Serial.printf("User event occurred %s\n",CSTR(msg)); });
}