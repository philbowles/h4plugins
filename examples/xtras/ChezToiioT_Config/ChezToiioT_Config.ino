#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

using DEVICE_MAP        =std::map<string,string>;
using FRIENDLY_MAP      =std::map<string,string>;
using SLAVE_MAP         =std::map<string,vector<string>>;
using SCHEDULE_MAP      =std::map<string,H4P_SCHEDULE>;

DEVICE_MAP  devices={
    {"F41FE8","cave"},  //1
    {"EA17C4","salon"}, //2
    {"F49D4B","eiffel"}, //3
    {"17D848","labrat"},//4
    {"17D383","dodo"},//5
    {"836EDC","chipper"}, //6
    {"A00759","solar1"}, //7
    {"A006CD","solar2"},//8
    {"FABFEA","click-l"},//9
    {"F90E13","princess"},//10
    {"F9198C","master"},//11
    {"F906B3","landing"},//12
    {"F9198B","middle"},//13
    {"0BC939","fourteen"},//14
    {"8EC098","mailbox"},//15
    {"BF713C","devkit"},
    {"0BAC73","S120"},
    {"5ACB8C","S126"}
};


FRIENDLY_MAP  friendlies={
    {"F41FE8","Cave Floor Lamp"},  //1
    {"EA17C4","Salon Corner Lamp"}, //2
    {"F49D4B","Salon Eiffel",}, //3
    {"17D848","Landing Wiry Wemos","landing"},//4
    {"17D383","Lab Testbed","landing"},//5
    {"836EDC","ChezToiioT Config",}, //6
    {"A00759","Solar Colombage",}, //7
    {"A006CD","Solar 2",},//8
    {"FABFEA","Landing clicker",},//9
    {"F90E13","Princess Suite",},//10
    {"F9198C","Master Bedroom Floor Lamp",},//11
    {"F906B3","Landing Floor Lamp",},//12
    {"F9198B","Middle Earth Floor Lamp",},//13
    {"0BC939","Lab phoenix",},//14
    {"8EC098","Mailbox",},//15
    {"BF713C","Devkit Thirty Two",},
    {"0BAC73","Ess one twenty",},
    {"5ACB8C","Ess one twenty six",}
};

SLAVE_MAP  slaves={
    {"labrat",{"master","landing"}}  //1
};

SCHEDULE_MAP={
    {"labrat",
        {"07:00","09:00"}
    },
    {"landing",
        {"21:00","23:59"}
    }
}; 

void retain(const string& topic, const string& value){

}

void onMqttConnect(){
    h4Chunker<DEVICE_MAP>(chips,[](DEVICE_MAP::const_iterator i){
        string chip=i->first;
        string device=i->second;
        Serial.printf("CHIP: %s device=%s\n",CSTR(chip),CSTR(device));
        /*
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
        */
    });
}

//H4P_SerialLogger h4sl;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","cheztoi");
H4P_AsyncWebServer h4asws;
H4P_AsyncMQTT h4mqtt("192.168.1.4",1883,"","",onMqttConnect);