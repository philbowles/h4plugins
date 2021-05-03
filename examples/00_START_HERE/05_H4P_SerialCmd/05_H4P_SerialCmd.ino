#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false)
/*
    Open Serial monitor and try typing any of the following:
   
h4/clear
h4/config
h4/dump
h4/factory
h4/get
h4/reboot
h4/show/all
h4/show/fs
h4/show/globals
h4/show/heap
h4/show/plugins
h4/show/q
h4/svc/info
h4/svc/restart
h4/svc/start
h4/svc/stop
help
*/

void h4setup() {
    Serial.println("H4P_SerialCmd Example v"H4P_VERSION);
    h4.every(1000,[]{ Serial.printf("T=%u ping\n",millis()); }); // ensure something in Q to see
    h4p.invokeCmd("help"); // Automatically show help at startup
    Serial.println("Show Q caused by invokeCmd");
    h4p.invokeCmd("h4/show/q"); // Automatically show H4 queue at startup
    Serial.println("Show Q caused by call to showQ()");
    h4p.showQ(); // same as above but more efficient
    if(h4p.invokeCmd("bogus/nonsense")) Serial.println("Invalid command invoked");
    Serial.println("Show all caused by call to all()");
    h4p.all(); // show everything
}