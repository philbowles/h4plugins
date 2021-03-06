/*
 MIT License

Copyright (c) 2020 Phil Bowles <H48266@gmail.com>
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
#pragma once

#include <H4PCommon.h>
#include<H4P_WiFi.h>
#include <asyncHTTPrequest.h>

#define H4P_MYSQL_HOLDOFF   5
/*

#define HTTPCODE_CONNECTION_REFUSED  (-1)
#define HTTPCODE_SEND_HEADER_FAILED  (-2)
#define HTTPCODE_SEND_PAYLOAD_FAILED (-3)
#define HTTPCODE_NOT_CONNECTED       (-4)
#define HTTPCODE_CONNECTION_LOST     (-5)
#define HTTPCODE_NO_STREAM           (-6)
#define HTTPCODE_NO_HTTP_SERVER      (-7)
#define HTTPCODE_TOO_LESS_RAM        (-8)
#define HTTPCODE_ENCODING            (-9)
#define HTTPCODE_STREAM_WRITE        (-10)
#define HTTPCODE_TIMEOUT             (-11)
*/

using H4P_FN_HTTPFAIL = function<void(int)>;
class H4P_HttpMySQLLogger: public H4Plugin, public asyncHTTPrequest {
        string          ip;
        H4P_FN_HTTPFAIL _fail;
        bool            inflight=false;
        bool            warning=false;

        void        _requestCB(void* optParm, asyncHTTPrequest* r, int readyState){ 
            if(readyState == 4) {
                inflight=false;
                if(r->responseHTTPcode() < 0){
                    if(!warning){
                        warning=true;
                        if(_fail) _fail(r->responseHTTPcode());
                    }
                } else warning=false;
            }
        }

        void        _handleEvent(H4PID pid,H4P_EVENT_TYPE type,const string &msg){
            static  uint32_t _logseq=0;
            if(!inflight){
                open("POST",CSTR(ip));
                inflight=true;
                char buf[256];
                sprintf(buf,"device=%s&msg=%s&type=%d&source=%d&seq=%u",CSTR(_cb[deviceTag()]),CSTR(msg),type,pid,_logseq++);
                setReqHeader("Content-Type","application/x-www-form-urlencoded");
                send(buf);
            } else h4.once(H4P_MYSQL_HOLDOFF,[=](){ _handleEvent(pid,type,msg); },nullptr,H4P_TRID_MLRQ);
        }

        void _greenLight() override {} // prevetn autostart - wait until wifi up

    public:
        H4P_HttpMySQLLogger(const string& ipaddress,H4P_FN_HTTPFAIL fnFail=nullptr,uint32_t filter=H4P_EVENT_ALL): 
            _fail(fnFail),ip(ipaddress),H4Plugin(H4PID_SQLL,filter){}
            
        void _hookIn() override { // protect
            h4pdepend<H4P_WiFi>(this,H4PID_WIFI);
//            setDebug(true);
            onReadyStateChange( [this](void* && a,asyncHTTPrequest* && b,int  && c){ _requestCB(a,b,c); });
        }
};