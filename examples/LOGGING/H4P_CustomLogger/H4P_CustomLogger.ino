#include<H4Plugins.h>
H4_USE_PLUGINS(115200,10,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_VerboseMessages h4vm;
H4P_SerialLogger h4sl; // defaults to H4P_EVENT_ALL

class myLogger: public H4Plugin {
        void _handleEvent(H4PID pid,H4P_EVENT_TYPE t,const string& msg) override {
            uint32_t msgvalue=atoi(msg.c_str());
            if(msgvalue%2) Serial.printf("That's odd! Type=%s from %s msg=%s\n",h4pgetEventName(t).c_str(),h4pnames[pid].c_str(),msg.c_str());
        }
    public:
        myLogger(uint32_t filter=H4P_EVENT_ALL): H4Plugin("lumberjack",filter){}
};

myLogger lumberjack(H4P_EVENT_USER);

void h4setup(){
  static uint32_t pingCount=0;
  h4.once(3000,[](){ 
    h4UserEvent("666"); 
    h4UserEvent("669"); 
  });
  h4.every(1000,[](){ h4UserEvent("%d",pingCount++); }); // sends H4P_EVENT_USER, so will get picked up
  h4.every(5000,[](){ SEVENT(H4P_EVENT_NOOP,"ignore me"); }); // SEVENT sends system event - ignored by YOU, caught by H4P_SerialLogger
}