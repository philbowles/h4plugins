#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop
//
//    Configuration
//
const char* WIFI_SSID="XXXXXXXX";
const char* WIFI_PASS="XXXXXXXX";
const char* MQTT_SERVER="http://192.168.1.4:1883";

H4P_SerialLogger woody;
H4P_EmitHeap sheep;
H4P_EmitQ cue(1000,5000); // every 1s, scale by 5000 so its still visible on heap graph
H4P_WiFi whiffy(WIFI_SSID,WIFI_PASS);
H4P_AsyncMQTT cutie(MQTT_SERVER);

H4P_MQTTLogger mqheap("heap",H4PE_HEAP); // name the topic
H4P_MQTTLogger mqq("Q",H4PE_Q); // name the topic

void h4setup(){
    h4pUserEvent("Log tester %d",666); // like printf
}