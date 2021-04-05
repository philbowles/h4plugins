#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_Signaller h4fc;
/*
Use for active-low LEDs e.g. ESP8266, ESP32.

Stops flashing after 1m
If you mix up active-high and active-low, LED will remain ON after 1m
instead of going OFF
*/
#define ULED_ACTIVE ACTIVE_LOW

void h4setup() { // H4 constructor starts Serial
    Serial.println("H4P_Signaller simple Example v"H4P_VERSION);
    h4fc.flashPattern("10",300,LED_BUILTIN,ULED_ACTIVE); // LED depends on platform

    h4.once(10000,[](){ 
      h4fc.flashPattern("10111110",250,LED_BUILTIN,ULED_ACTIVE); // LED depends on platform
    });
    h4.once(20000,[](){ 
      h4fc.flashPattern("11011011000",200,LED_BUILTIN,ULED_ACTIVE); // LED depends on platform
    });
    h4.once(30000,[](){ 
      h4fc.flashPattern("1100011111110000000000",150,LED_BUILTIN,ULED_ACTIVE); // LED depends on platform
    });
    h4.once(40000,[](){ 
      h4fc.flashPattern("1000000000000000000000",100,LED_BUILTIN,ULED_ACTIVE); // LED depends on platform
    });

    h4.once(60000,[](){ 
        h4fc.stopPin(LED_BUILTIN);
        Serial.print(millis());Serial.println(" LED_SHOULD BE OFF");
        h4p.all();
    });
}