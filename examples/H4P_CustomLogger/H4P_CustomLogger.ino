#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

class myLogger: public H4PLogService {
        void        _logEvent(const string &msg,H4P_LOG_TYPE type,const string& source,const string& target){
            Serial.print("myLogger ");
            Serial.print(millis());
            Serial.print(" ");
            Serial.println(msg.c_str()); // or  Serial.println(CSTR(msg));
        }
    public:
        myLogger(): H4PLogService("mylog"){}
};



H4P_CmdErrors h4ce;


//H4P_SerialLogger h4sl;
myLogger lumberjack;

void h4setup(){
  static uint32_t pingCount=0;
  h4UserEvent("test1 %d\n",666);
  h4UserEvent("Ztest2");
  h4.every(1000,[](){ h4UserEvent("ping %d",pingCount++); });
}