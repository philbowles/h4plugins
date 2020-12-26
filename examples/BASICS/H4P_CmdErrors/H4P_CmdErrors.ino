#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_CmdErrors h4ce; // must be created BEFORE any other plugins
H4P_SerialLogger h4sl; // not essential but helps to see whats going on

/*
    Open Serial monitor and try typing any of the following (plus any additional parameters):

    h4/dump
    h4/factory
    h4/reboot
    h4/show/all   <--- try this one first
    h4/show/config
    h4/show/fs
    h4/show/heap
    h4/show/plugins
    h4/show/q
    h4/svc/info
    h4/svc/restart
    h4/svc/start
    h4/svc/stop
    help

    Then type a bogus command, any old junk will do

*/
void h4setup() { // H4 constructor starts Serial
    Serial.println("H4P_CmdErrors Example v"H4P_VERSION);

    h4.every(1000,[]{}); // do nothing, but have something in Q to see!
    
    h4cmd.invokeCmd("help"); // Automatically show help at startup
    Serial.println("Show Q caused by invokeCmd");
    h4cmd.invokeCmd("h4/show/q"); // Automatically show H4 queue at startup
    Serial.println("Show Q caused by call to showQ()");
    h4cmd.showQ(); // same as above but more efficient

    uint32_t  errorCode=h4cmd.invokeCmd("bogus/nonsense");
    if(errorCode) Serial.println((h4ce.getErrorMessage(errorCode)).c_str());
    Serial.println("Show all caused by call to all()");
    h4cmd.all(); // show everything
}