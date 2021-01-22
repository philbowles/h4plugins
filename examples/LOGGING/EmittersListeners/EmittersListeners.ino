#include<H4Plugins.h>
H4_USE_PLUGINS(115200,10,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_VerboseMessages h4vm;
// event emitters
H4P_EmitTick h4eet; // 1x per second sends H4P_EVENT_HEARTBEAT
H4P_EmitHeap h4eeh(5000); // every 5 seconds sends H4P_EVENT_HEAP
H4P_EmitQ h4eeq(2000); // H4P_EVENT_Q every 2secs

// event listeners 
// All loggers are event listeners, they simply "foward" the messages somewhere useful
H4P_SerialLogger h4sl(H4P_EVENT_HEAP | H4P_EVENT_Q); // only react to heap and Q

H4P_EventListener h4elh("hear",H4P_EVENT_HEARTBEAT,[](const string& msg){ Serial.printf("User can hear your heartbeat %s\n",CSTR(msg)); });
H4P_EventListener h4elu("user",H4P_EVENT_USER,[](const string& msg){ Serial.printf("User sez %s\n",CSTR(msg)); });
H4P_EventListener h4elx("usr2",H4P_EVENT_ALL &~ H4P_EVENT_USER,[](const string& msg){ Serial.printf("Usr2 sez %s\n",CSTR(msg)); });

void h4setup(){
  h4.everyRandom(5000,10000,[](){ h4UserEvent("wow signal @ %u",millis()); });
}
