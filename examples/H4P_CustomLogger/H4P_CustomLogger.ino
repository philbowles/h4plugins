#include<H4Plugins.h>
H4_USE_PLUGINS

class myLogger: public H4PLogService {
        void        _logEvent(const string &msg,H4P_LOG_TYPE type,const string& source,const string& target,uint32_t error){
            if(_running) {
                Serial.print("myLogger ");
                Serial.print(millis());
                Serial.print(" ");
                Serial.println(msg.c_str()); // or  Serial.println(CSTR(msg));
            }
        }
    public:
        myLogger(): H4PLogService("mylog"){
            subid=H4PC_MYLOG; // see note below
            _names={ {H4P_TRID_MYLOG,uppercase(_pid)} }; // see note below
        }
};

H4 h4(115200);
H4P_SerialCmd h4sc;
#ifndef ARDUINO_ARCH_STM32
  H4P_LocalLogger h4ll(10000); // 10k = amount of SPIFFS to use ESP8266 / ESP32 only
#endif
H4P_SerialLogger h4sl;
mylogger mine;

void h4setup(){
  h4sc.logEvent("normal call");
  EVENT("test1"); // exactly the same, but can't be "compiled out"
  EVENT("Ztest2"); // and its less typing
}
