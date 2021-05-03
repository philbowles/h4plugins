#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

/*

Let it run for a while, then try some of the following:

h4/svc/info/llog ( to view the permanently saved log )
h4/llog/flush ( to view then clear the permanently saved log )
h4/llog/clear ( to empty the permanently saved log )
h4/llog/msg/any old data you like to add your own message to the log

also try h4/reboot followed by h4/svc/info/llog - all the messages from the last run will be there
also try h4/factory followed by h4/svc/info/llog - all previous messages are erased
 */
H4P_LocalLogger local(10000); // 10k = amount of LittleFS to use

void h4setup(){
  static uint32_t pingCount=0;
  h4.once(3000,[](){ h4pUserEvent("Odds %d",669); });
  h4.every(1000,[](){ h4pUserEvent("PING %d",pingCount++); }); // sends H4PE_USER, so will get picked up
  h4.every(5000,[](){ h4psysevent("user",H4PE_SYSINFO,"ignore me %d",millis()); }); // will get ignored by YOU
}