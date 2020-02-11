#include<H4Plugins.h>
H4_USE_PLUGINS

H4 h4(115200); //auto-start Serial @ 115200, default Q size=20 

H4P_CmdErrors h4ce; // must be created BEFORE H4P_SerialCmd
H4P_SerialCmd h4sc;

/*
    Open Serial monitor and try typing any of the following:

    * h4/reboot
    * h4/show/Q
    * h4/show/Qstats
    * h4/show/all
    * h4/show/tnames
    * h4/show/unload
    * h4/unload
    * help

*/
void h4setup() { // H4 constructor starts Serial
    Serial.println("H4P_CmdErrors Example v"H4P_VERSION);

    h4sc.invokeCmd("help"); // Automatically show help at startup
    Serial.println("Show Q caused by invokeCmd");
    h4sc.invokeCmd("h4/show/Q"); // Automatically show H4 queue at startup
    Serial.println("Show Q caused by call to dumpQ()");
    h4sc.dumpQ(); // same as above but more efficient

    uint32_t  errorCode=h4sc.invokeCmd("bogus/nonsense");
    if(errorCode) Serial.println((h4ce.getErrorMessage(errorCode)).c_str());
    Serial.println("Show all caused by call to all()");
    h4sc.all(); // show everything
}
