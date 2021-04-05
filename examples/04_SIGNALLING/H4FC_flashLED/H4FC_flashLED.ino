#define H4P_VERBOSE 1
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_Signaller h4fc;
/*
Use for active-low LEDs e.g. ESP8266, ESP32.

Stops flashing after 1m
If you mix up active-high and active-low, LED will remain ON after 1m
instead of going OFF
*/

void h4setup() { // H4 constructor starts Serial
    Serial.println("H4P_Signaller Integrated Example v"H4P_VERSION);
    h4fc.flashPin(1024,LED_BUILTIN,H4P_ASSUMED_SENSE); // LED depends on platform

    h4.once(10000,[](){ 
      h4fc.flashPin(512,LED_BUILTIN,H4P_ASSUMED_SENSE); // LED depends on platform
    });
    h4.once(20000,[](){ 
      h4fc.flashPin(256,LED_BUILTIN,H4P_ASSUMED_SENSE); // LED depends on platform
    });
    h4.once(30000,[](){ 
      h4fc.flashPin(128,LED_BUILTIN,H4P_ASSUMED_SENSE); // LED depends on platform
    });
    h4.once(40000,[](){ 
      h4fc.flashPin(64,LED_BUILTIN,H4P_ASSUMED_SENSE); // LED depends on platform
    });
    h4.once(60000,[](){ 
        h4fc.stopPin(LED_BUILTIN);
        Serial.print(millis());Serial.println(" LED_SHOULD BE OFF");
        h5cmd.all();
    });
}