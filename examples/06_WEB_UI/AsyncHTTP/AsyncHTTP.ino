#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop
//
// See https://github.com/philbowles/mysqlrest for a webserver that will
// store the events being logged here 
//
//  see also the RemoteLogger example
//
#define URL "http://192.168.1.20:8266"

H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","bogus");
H4P_AsyncHTTP ah;

VARK_NVP_MAP    _eventdata = {
    {deviceTag(),"bogus"},
    {"type","42"},
    {"source","evian"},
    {"msg",""}
};

void h4setup(){
  static uint32_t pingCount=0;

  h4.every(5000,[](){ h4pUserEvent("%d",pingCount++); });
  h4.every(500,[](){ 
    _eventdata["msg"]=stringFromInt(millis());
    ah.POST(URL,_eventdata,[](ARMA_HTTP_REPLY r){
          if(r.httpResponseCode < 200 || r.httpResponseCode > 299){
              Serial.printf("HTTP Response Code: %d\n",r.httpResponseCode);
              Serial.printf("Response Headers:\n");
              for(auto const h:r.responseHeaders) Serial.printf("%s=%s\n",h.first.data(),h.second.data());
              Serial.printf("\nRaw Data\n");
              dumphex(r.data,r.length);
              Serial.printf("\nAs a std::string - BE CAREFUL, IT MAY NOT BE A STRING!!!\n%s\n",r.asStdstring().data()); // Data may NOT be a string -> crash!!!
          }
      });
  });
}