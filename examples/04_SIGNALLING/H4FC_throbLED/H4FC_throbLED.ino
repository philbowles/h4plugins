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
    Serial.println("H4P_Signaller Throb Example v"H4P_VERSION);
    h4fc.throbPin(500,H4P_ASSUMED_LED,H4P_ASSUMED_SENSE); // LED depends on platform

    h4.once(10000,[](){ 
      h4fc.throbPin(750,H4P_ASSUMED_LED,H4P_ASSUMED_SENSE); // LED depends on platform
    });
    
    h4.once(20000,[](){ 
      h4fc.throbPin(1000,H4P_ASSUMED_LED,H4P_ASSUMED_SENSE); // LED depends on platform
    });
    h4.once(30000,[](){ 
      h4fc.throbPin(1500,H4P_ASSUMED_LED,H4P_ASSUMED_SENSE); // LED depends on platform
    });
    h4.once(40000,[](){ 
      h4fc.throbPin(2000,H4P_ASSUMED_LED,H4P_ASSUMED_SENSE); // LED depends on platform
    });
    h4.once(60000,[](){ 
        h4fc.stopPin(H4P_ASSUMED_LED);
        Serial.print(millis());Serial.println(" LED_SHOULD BE OFF");
        h4p.all();
    });
}