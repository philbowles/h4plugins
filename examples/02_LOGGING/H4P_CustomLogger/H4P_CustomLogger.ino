#define H4P_VERBOSE 1
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_SerialLogger h4sl; // defaults to H4PE_ALL

class myLogger: public H4Service {
        void _handleEvent(const std::string& svc,H4PE_TYPE t,const std::string& msg) override {
            uint32_t msgvalue=atoi(msg.c_str());
            if(msgvalue%2) Serial.printf("That's odd! Type=%s from %s msg=%s\n",h4pGetEventName(t).c_str(),svc.c_str(),msg.c_str());
        }
    public:
        myLogger(uint32_t filter=H4PE_ALL): H4Service("lumberjack",filter){}
};

myLogger lumberjack(H4PE_MSG);

void h4setup(){
  static uint32_t pingCount=0;
  h4.once(3000,[](){ 
    h4pUserEvent("Evens %d",666); 
    h4pUserEvent("Odds %d",669); 
  });
  h4.every(1000,[](){ h4pUserEvent("%d",pingCount++); }); // sends H4PE_MSG, so will get picked up
  h4.every(5000,[](){ h4psysevent("user",H4PE_SYSINFO,"ignore me %d",millis()); }); // will get ignored by YOU
}