#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop
/*
    Open Serial monitor and try typing any of the following:

    h4/dump
    h4/factory
    h4/reboot
    h4/show/all
    h4/show/config
    h4/show/fs
    h4/show/heap
    h4/show/plugins
    h4/show/q
    h4/svc/info
    h4/svc/restart
    h4/svc/start
    h4/svc/stop

*/

void h4setup() { // H4 constructor starts Serial
    Serial.println("H4P_SerialCmd Example v"H4P_VERSION);
    h4.every(1000,[]{ Serial.printf("T=%u ping\n",millis()); }); // ensure something in Q to see
    h4cmd.invokeCmd("help"); // Automatically show help at startup
    Serial.println("Show Q caused by invokeCmd");
    h4cmd.invokeCmd("h4/show/q"); // Automatically show H4 queue at startup
    Serial.println("Show Q caused by call to showQ()");
    h4cmd.showQ(); // same as above but more efficient
    if(h4cmd.invokeCmd("bogus/nonsense")) Serial.println("Invalid command invoked");
    Serial.println("Show all caused by call to all()");
    h4cmd.all(); // show everything
}
