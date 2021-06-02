/*
 MIT License

Copyright (c) 2020 Phil Bowles <h4plugins@gmail.com>
   github     https://github.com/philbowles/esparto
   blog       https://8266iot.blogspot.com     
   groups     https://www.facebook.com/groups/esp8266questions/
              https://www.facebook.com/Esparto-Esp8266-Firmware-Support-2338535503093896/
                			  

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
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
#include<H4P_RemoteLogger.h>
#include<H4P_SerialCmd.h>

H4P_RemoteLogger::H4P_RemoteLogger(const std::string& url,uint32_t filter): _url(url), H4P_AsyncHTTP("rlog",filter) { _eventdata[deviceTag()]=h4p[deviceTag()]; }
//
//      our raison d'etre
//
void H4P_RemoteLogger::_handleEvent(const std::string& svc,H4PE_TYPE t,const std::string& msg) {
    if(_running && svc!=_me){
        _eventdata["source"]=svc;
        _eventdata["type"]=stringFromInt(static_cast<uint32_t>(t));
        _eventdata["msg"]=msg;
        POST(_url,_eventdata,[](ARMA_HTTP_REPLY r){
/*
            Serial.printf("H4P_RL RCVD T=%u %d\n",millis(),r.httpResponseCode);
            for(auto const h:r.responseHeaders) Serial.printf("%s=%s\n",h.first.data(),h.second.data());
            if(r.httpResponseCode < 200 || r.httpResponseCode > 299){
                Serial.printf("HTTP Response Code: %d\n",r.httpResponseCode);
                Serial.printf("Response Headers:\n");
                for(auto const h:r.responseHeaders) Serial.printf("%s=%s\n",h.first.data(),h.second.data());
                Serial.printf("\nRaw Data\n");
                dumphex(r.data,r.length);
                Serial.printf("\nAs a std::string - BE CAREFUL, IT MAY NOT BE A STRING!!!\n%s\n",r.asStdstring().data()); // Data may NOT be a std::string -> crash!!!
            }
*/
        });
    }
}