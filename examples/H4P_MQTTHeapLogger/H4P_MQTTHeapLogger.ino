#include<H4Plugins.h>
H4_USE_PLUGINS

H4 h4(115200);
H4P_SerialCmd h4sc;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","pinger");
H4P_SerialLogger h4sl;
H4P_MQTT h4mqtt("192.168.1.4",1883);
H4P_MQTTHeapLogger h4mh(1000); // publish heap value 1x per second
H4P_MQTTQueueLogger h4mq(1000); // publish queue size 1x per second
