#include<H4Plugins.h>
H4_USE_PLUGINS

H4 h4(115200);
H4P_SerialCmd h4sc;

//H4P_SerialLogger h4sl;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","mysqllogger");
H4P_MQTT h4mqtt("192.168.1.4",1883);
//H4P_MQTTHeapLogger h4mh(1000); // publish hep value 1x per second
H4P_MQTTQueueLogger h4mq(250); // publish hep value 1x per second
//H4P_HttpMySQLLogger h4mysql("http://192.168.1.20:8266/"); // trailing slash!!!

uint32_t serno=3;

void h4setup(){
  h4sc.logEvent("normal call");
  EVENT("test1"); // exactly the same, but can't be "compiled out"
  EVENT("Ztest2"); // and its less typing
  h4.every(25,[](){ EVENT("%u",serno++); });
}