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
#ifndef H4P_HttpMySQLLogger_HO
#define H4P_HttpMySQLLogger_HO

#ifndef ARDUINO_ARCH_STM32
#include <H4PCommon.h>
#include <asyncHTTPrequest.h>

class H4P_HttpMySQLLogger: public H4PLogService {
        asyncHTTPrequest request;

        void requestCB(void* optParm, asyncHTTPrequest* request, int readyState){
            if(readyState == 4){
                Serial.println(request->responseText());
                Serial.println();
                request->setDebug(false);
            }
        }
        //
        void        _logEvent(const string &msg,H4P_LOG_TYPE type,const string& source,const string& target,uint32_t error){
            request.open("POST", "http://192.168.1.22:8266");
            static char buf[256];
            sprintf(buf,"msg=%s&type=%d&source=%s&target=%s&error=%d\r\n\r\n",CSTR(msg),type,CSTR(source),CSTR(target),error);
            Serial.printf("H=%u %s\n",ESP.getFreeHeap(),buf);
            request.send(buf);
        }
        void _hookIn() override { // protect
            if(H4Plugin::isLoaded(wifiTag())){
                H4PLogService::_hookIn();
                h4wifi.hookConnect([this](){ start(); });
                h4wifi.hookDisconnect([this](){ stop(); });
                request.setDebug(true);
                request.onReadyStateChange(bind(&H4P_HttpMySQLLogger::requestCB,this,_1,_2,_3));
            } 
            else { DEPENDFAIL(wifi); }
        }
    public:
        H4P_HttpMySQLLogger(uint32_t filter=H4P_LOG_ALL): H4PLogService("hlog",filter){}
};
#endif
#endif // H4P_HttpMySQLLogger_H