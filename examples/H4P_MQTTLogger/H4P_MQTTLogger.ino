#include<H4Plugins.h>
H4_USE_PLUGINS

H4 h4(115200);
H4P_SerialCmd h4sc;
H4P_GPIOManager h4gm;
H4P_FlasherController h4fc;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","pinger");
H4P_SerialLogger h4sl;
H4P_MQTTLogger h4mlog1("hazlenuts",H4P_LOG_USER); // only log user events
H4P_MQTTLogger h4mlog2("pecans"); // name the topic

H4_TIMER mqttPinger; 
void onMQTTConnect(){
    EVENT("%u Testing the %s Logger",millis(),"variadic");
    mqttPinger=h4.every(60000,[](){ EVENT("something happened"); });
}
void onMQTTDisconnect(){
    h4.cancel(mqttPinger);
}

H4P_MQTT h4mqtt("192.168.1.4",1883,"","",onMQTTConnect,onMQTTDisconnect);

H4P_AsyncWebServer h4asws("admin","admin");

void h4setup(){
    h4sc.logEvent("Log tester %d",666); // like printf
}