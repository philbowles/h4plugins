#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop


H4P_GPIOManager h4gm;
H4P_FlasherController h4fc;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","pinger");
H4P_SerialLogger h4sl;
H4P_MQTTLogger h4mlog1("hazelnuts",H4P_LOG_USER); // only log user events
H4P_MQTTLogger h4mlog2("pecans"); // name the topic

H4_TIMER mqttPinger; 
void onMQTTConnect(){
    h4UserEvent("%u Testing the %s Logger",millis(),"variadic");
    mqttPinger=h4.every(60000,[](){ h4UserEvent("something happened"); });
}
void onMQTTDisconnect(){
    h4.cancel(mqttPinger);
}

H4P_AsyncMQTT h4mqtt("192.168.1.4",1883,"","",onMQTTConnect,onMQTTDisconnect);

H4P_AsyncWebServer h4asws;



void h4setup(){
    h4UserEvent("Log tester %d",666); // like printf
}