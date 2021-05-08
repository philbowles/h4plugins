#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop
/*
 * Try Serial commands
 * 
 * h4/on
 * h4/off
 * h4/toggle
 * h4/switch/n where n is 0 or 1
 * 
 */
H4P_BinarySwitch h4onof(LED_BUILTIN,H4P_ASSUMED_SENSE);

void h4setup() { // H4 constructor starts Serial
    Serial.println("H4P_BinarySwitch Simple Example v"H4P_VERSION);
    h4.once(5000,[](){ h4onof.turnOn(); });
    h4.once(10000,[](){ h4onof.turnOff(); });
    h4.once(15000,[](){ h4onof.toggle(); });
    h4.once(20000,[](){ h4onof.turn(0); });
}