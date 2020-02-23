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
#include<H4P_WiFiSelect.h>
#include<H4P_WiFi.h>
#include <asyncHTTPrequest.h>

#define H4P_MYSQL_HOLDOFF   1

using H4P_FN_HTTPFAIL = function<void(int)>;

class H4P_HttpMySQLLogger: public H4PLogService {
        asyncHTTPrequest request;
        string          ip;
        H4P_FN_HTTPFAIL _fail;
        bool            inflight=false;
        bool            warning=false;

        void requestCB(void* optParm, asyncHTTPrequest* request, int readyState){ 
//            Serial.printf("RS=%d http Error code = %d txt=%s\n",readyState,request->responseHTTPcode(),CSTR(request->responseText()));
            if(readyState == 4) {
                inflight=false;
                if(request->responseHTTPcode() < 0){
                   if(!warning){
                       warning=true;
                        if(_fail) _fail(request->responseHTTPcode());
                   }
                } else warning=false;
            }
        }

        void        _logEvent(const string &msg,H4P_LOG_TYPE type,const string& source,const string& target){
            static  uint32_t _logseq=0;
            if(!inflight){
//                if(request.open("POST",CSTR(ip))){
                request.open("POST",CSTR(ip));
                inflight=true;
                char buf[256];
                sprintf(buf,"device=%s&msg=%s&type=%d&source=%s&target=%s&seq=%u",CSTR(_cb[deviceTag()]),CSTR(msg),(int) type,CSTR(source),CSTR(target),_logseq++);
                request.setReqHeader("Content-Type","application/x-www-form-urlencoded");
                request.send(buf);
 //              } 
            } else h4.once(H4P_MYSQL_HOLDOFF,bind(&H4P_HttpMySQLLogger::_logEvent,this,msg,type,source,target),nullptr,H4P_TRID_MLRQ);
        }

        void _hookIn() override { // protect
            if(H4Plugin::isLoaded(wifiTag())){
                H4PLogService::_hookIn();
                h4wifi.hookConnect([this](){ start(); });
                h4wifi.hookDisconnect([this](){ stop(); });
//                request.setDebug(true);
                request.onReadyStateChange(bind(&H4P_HttpMySQLLogger::requestCB,this,_1,_2,_3));
            } else { DEPENDFAIL(wifi); }
        }
    protected:
        void _greenLight() override {} // prevetn autostart - wait until mqtt up
    public:
        H4P_HttpMySQLLogger(const string& ipaddress,H4P_FN_HTTPFAIL fnFail=nullptr,uint32_t filter=H4P_LOG_ALL): 
            _fail(fnFail),ip(ipaddress),H4PLogService("mysql",filter){
                _names={{H4P_TRID_MLRQ,"MLRQ"}};
        }
};
#endif
#endif // H4P_HttpMySQLLogger_H