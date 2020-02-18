#include<H4Plugins.h>
H4_USE_PLUGINS

class myLogger: public H4PLogService {
        void        _logEvent(const string &msg,H4P_LOG_TYPE type,const string& source,const string& target,uint32_t error){
            Serial.print("myLogger ");
            Serial.print(millis());
            Serial.print(" ");
            Serial.println(msg.c_str()); // or  Serial.println(CSTR(msg));
        }
    public:
        myLogger(): H4PLogService("mylog"){}
};

H4 h4(115200);
H4P_SerialCmd h4sc;
H4P_SerialLogger h4sl;
myLogger lumberjack;

void h4setup(){
  h4sc.logEvent("normal call");
  EVENT("test1"); // exactly the same, but can't be "compiled out"
  EVENT("Ztest2"); // and its less typing
}