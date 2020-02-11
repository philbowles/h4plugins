#include<H4Plugins.h>
H4_USE_PLUGINS

H4 h4(115200); //auto-start Serial @ 115200, default Q size=20 

H4P_FlasherController h4fc;
/*
Use for active-low LEDs e.g. ESP8266, ESP32.

Stops flashing after 10sec
If you mix up active-high and active-low, LED will remain ON after 10sec
instead of going OFF
*/
void h4setup() { // H4 constructor starts Serial
    Serial.println("H4P_FlasherController Active Low Example v"H4P_VERSION);
    pinMode(LED_BUILTIN,OUTPUT);

    h4.once(10000,[](){ 
        h4fc.stopLED(LED_BUILTIN);
        Serial.print(millis());Serial.println(" LED_SHOULD BE OFF");
    });

    h4fc.flashLED(250,LED_BUILTIN,LOW); // LED is active LOW
}