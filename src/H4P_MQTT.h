/*
 MIT License

Copyright (c) 2019 Phil Bowles <H48266@gmail.com>
   github     https://github.com/philbowles/H4
   blog       https://8266iot.blogspot.com
   groups     https://www.facebook.com/groups/esp8266questions/
              https://www.facebook.com/H4-Esp8266-Firmware-Support-2338535503093896/


Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software iss
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
#ifndef H4P_MQTT_HO
#define H4P_MQTT_HO

#include<H4PCommon.h>
#include<H4P_SerialCmd.h>
#include<H4P_WiFiSelect.h>
#include<H4P_WiFi.h>
#ifndef H4P_NO_WIFI

#include<WiFiClient.h>
#include<PubSubClient.h>

class H4P_MQTT: public H4PluginService, public PubSubClient{
        unordered_set<string>   _grid;

        string          device;

        VSCMD(_change);
        VSCMD(_offline);
        VSCMD(_online);

            WiFiClient      _wifiClient;

                void         _forceDisconnect(bool autorestart=true);
                void         _hookIn() override;
                void         _setup();

    public:
        H4P_MQTT(string broker,uint16_t port, string user="",string pass="",H4_FN_VOID onC=[](){},H4_FN_VOID onD=[](){}):
            H4PluginService(onC,onD)
        {
            _cb["broker"]=broker;
            _cb[portTag()]=stringFromInt(port);
            _cb["muser"]=user,
            _cb["mpasswd"]=pass;

            _pid=mqttTag();

            _names={ 
                {H4P_TRID_MQMS,"MQMS"},
                {H4P_TRID_MQRC,"MQRC"},
                {H4P_TRID_MQTT,uppercase(_pid)}
            };

            _local={
                {"change",  { subid, 0, CMDVS(_change) }},            
                {"grid",    { subid, 0, CMD(showGrid) }},            
                {"offline", { subid, 0, CMDVS(_offline) }},            
                {"online",  { subid, 0, CMDVS(_online) }}           
            };       
        }
                void        change(const string& broker,uint16_t port);
                void        publishDevice(const string& topic,const string& payload="");
                void        publishDevice(const string& topic,uint32_t payload){ publishDevice(topic,stringFromInt(payload)); }
                void        restart() override { _forceDisconnect(true); };
                void        showGrid(){ for(auto const& g:_grid) reply("%s\n",CSTR(g)); }
                void        start() override;
                void        stop(){ _forceDisconnect(false); };
                void        subscribeDevice(string topic,H4_FN_MSG f);
                void        unsubscribeDevice(string topic);

    //          syscall only                               
                void        _reply(string msg) override { publish(CSTR(string("h4/"+device+"/reply")),CSTR(msg)); }
};

    extern __attribute__((weak)) H4P_MQTT h4mqtt;
#endif

#endif // H4P_MQTT_H