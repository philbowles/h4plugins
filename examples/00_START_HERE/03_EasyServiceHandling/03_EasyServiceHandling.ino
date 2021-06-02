#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop
//
//  You may need to react to services starting and/or stopping, for example
//    you must subscribe to MQTT topics when the MQTT service starts 
//    (formerly the onMqttConnect callback function)
//  Thus you need to convert the message to an integer, than call a connect function if it's 1
//    or a disconnect function if it's 0
//  
//  The H4P_SERVICE_ADAPTER( x ) does this for you, just replace x with the service name
//  NB whatver you put into x is converted to lowercase, so you can "prettify" your functions
//  if you want to -for example H4P_SERVICE_ADAPTER(Mqtt) will listen for service 'mqtt' but
//  call MqttConnect and/or MqttDisconnect
//
H4P_Signaller blinky;
H4P_WiFi wifi("XXXXXXXX","XXXXXXXX","easysvc");
H4P_AsyncMQTT mqtt("http://192.168.1.4:1883");

void h4pGlobalEventHandler(const std::string& svc,H4PE_TYPE t,const std::string& msg){
  //Serial.printf("Big Brother sees EVERYTHING %s %s %s\n",CSTR(svc),CSTR(h4pGetEventName(t)),CSTR(msg));
  switch(t){
    H4P_DEFAULT_SYSTEM_HANDLER
    case H4PE_SERVICE:
      H4P_SERVICE_ADAPTER(WIFI);
      H4P_SERVICE_ADAPTER(Mqtt);
      break;
  }   
}

void onWIFIConnect(){ Serial.printf("HURRAH: WIFI connected\n"); }
void onWIFIDisconnect(){ Serial.printf("OH DEAR: Router Offline?\n"); }

void onMqttConnect(){
    Serial.printf("USER: MQTT connected\n");
    mqtt.subscribeDevice("mytopic",[](std::vector<std::string> vs){
      Serial.printf("USER: RX payload=%s\n",H4PAYLOAD.c_str()); // convert payload to C-style string
      return H4_CMD_OK;
    });
}

void onMqttDisconnect(){}