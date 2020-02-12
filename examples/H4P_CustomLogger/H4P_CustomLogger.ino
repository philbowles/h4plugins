#include<H4Plugins.h>
H4_USE_PLUGINS
/*

**Note** to get the next values for `H4PC_MYLOG` and `H4P_TRID_MYLOG` you need to edit `H4PCommon.h`

```cpp
...
  H4P_TRID_NTFY,
  H4P_TRID_UBSW,
  H4P_TRID_3FNB,
  H4P_TRID_CERR,
  H4P_TRID_SCMD,
  H4P_TRID_QWRN,
  H4P_TRID_SNIF,
  H4P_TRID_LLOG,
  H4P_TRID_SLOG,
  H4P_TRID_CURL, // comma added by you
  H4P_TRID_MYLOG // ADD THIS HERE and put a comma at the end of the line above
};

enum H4PC_CMD_ID{
    H4PC_ROOT=1,
    H4PC_SHOW,
    subid,
    H4PC_QWRN,
    subid,
    H4PC_ESW_SET,
    H4PC_ESW_SWEEP,
    H4PC_WIFI,
    H4PC_MQTT,
    H4PC_ASWS,
    H4PC_SPIF,
    H4PC_UPNP, 
    H4PC_LLOG, 
    H4PC_SLOG,
    H4PC_CURL, // comma added by you
    H4PC_MYLOG // ADD THIS HERE and put a comma at the end of the line above
};
...
 */

 // this is a quick and dirty fix to allow this example to compile 
 // - DO NOT use these values in real life, see above!
 #define H4P_TRID_MYLOG 49
 #define H4PC_MYLOG 49
 //
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
//  H4P_LocalLogger h4ll(10000); // 10k = amount of SPIFFS to use ESP8266 / ESP32 only
#endif
H4P_SerialLogger h4sl;
myLogger lumberjack;

void h4setup(){
  h4sc.logEvent("normal call");
  EVENT("test1"); // exactly the same, but can't be "compiled out"
  EVENT("Ztest2"); // and its less typing
}