#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

struct widget{
  string device;
  string alexa;
  vector<string> slaves;
};

typedef std::map<string,widget> CHIP_MAP;

CHIP_MAP  chips={
 {"F41FE8",{"cave","Cave Floor Lamp",{}}},  //1
 {"EA17C4",{"salon","Salon Corner Lamp",{"eiffel"}}}, //2
 {"F49D4B",{"eiffel","Salon Eiffel",{}}}, //3
 {"17D848",{"labrat","Landing Wiry Wemos",{"master","landing"}}},//4
 {"17D383",{"dodo","Lab Testbed",{"master","landing"}}},//5
 {"836EDC",{"chipper","ChezToiioT Config",{}}}, //6
 {"A00759",{"solar1","Solar Colombage",{}}}, //7
 {"A006CD",{"solar2","Solar 2",{}}},//8
 {"FABFEA",{"click-l","Landing clicker",{}}},//9
 {"F90E13",{"princess","Princess Suite",{}}},//10
 {"F906B3",{"master","Master Bedroom Floor Lamp",{}}},//11
 {"F9198C",{"landing","Landing Floor Lamp",{}}},//12
 {"F9198B",{"middle","Middle Earth Floor Lamp",{}}},//13
 {"0BC939",{"fourteen","Lab phoenix",{}}},//14
 {"8EC098",{"mailbox","Mailbox",{}}}//15
};

void onMqttConnect(){
  h4Chunker<CHIP_MAP>(chips,[](CHIP_MAP::const_iterator i){
      string chip=i->first;
      widget w=i->second;
      string rawname="H4-"+chip;
      Serial.printf("Fixing up %s rename to %s/%s\n",CSTR(rawname),CSTR(w.device),CSTR(w.alexa));
      string topic=rawname+"/h4/wifi/host2";
      string payload=w.device+","+w.alexa;
      h4mqtt.publish(CSTR(topic), 0, true, CSTR(payload));
      for(auto s:w.slaves){
        topic=w.device+"/h4/slave/"+s;
        Serial.printf("%s slaved to %s top=%s\n",CSTR(s),CSTR(w.device),CSTR(topic));
        h4mqtt.publish(CSTR(topic), 0, true,CSTR(stringFromInt(1)));
      }
    });
}

//H4P_SerialLogger h4sl;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","cheztoi");
H4P_AsyncWebServer h4asws("admin","admin");
H4P_AsyncMQTT h4mqtt("192.168.1.4",1883,"","",onMqttConnect);