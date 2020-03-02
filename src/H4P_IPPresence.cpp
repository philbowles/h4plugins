/*
 MIT License

Copyright (c) 2019 Phil Bowles <h4plugins@gmail.com>
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


// NB won't work until https://github.com/esp8266/Arduino/issues/7117 is fixed


#include<H4P_IPPresence.h>
#ifndef H4P_NO_WIFI

extern "C" {
  #include <ping.h>
}

void H4P_IPPresence::_hookIn() {
    Serial.printf("_hookIn this=%08x @_ping=%08x\n",this,&_ping);
    if(H4Plugin::isLoaded(wifiTag()) ){
        h4wifi.hookConnect([this](){ start(); });
        h4wifi.hookDisconnect([this](){ stop(); });
    } else { DEPENDFAIL(wifi); }
}

void H4P_IPPresence::start(){
    Serial.printf("H4P_IPPresence::start on %s\n",CSTR(_ip));
    Serial.printf("this=%08x @_ping=%08x\n",this,&_ping);
    _pinger=h4.every(H4P_IPPD_RATE,[this](){
        Serial.printf("T=%u Pinging %s inst=%d prev state was %d\n",millis(),CSTR(_ip),_ping,_present);
        Serial.printf("lambda this=%08x @_ping=%08x\n",this,&_ping);
        struct ping_option *ping_opt = NULL;
        ping_opt = (struct ping_option *)os_zalloc(sizeof(struct ping_option));
        ping_opt->count = 1;    //  try to ping how many times
        ping_opt->coarse_time = 1;  // ping interval
        ping_opt->ip = ipaddr_addr(_ip.c_str());
        ping_regist_recv(ping_opt,reinterpret_cast<ping_recv_function>(&H4P_IPPresence::_ping_recv_cb));
        ping_start(ping_opt);

//        if(_ping!=_present) {
//            Serial.printf("ELVIS %d\n",_ping);
//            _present=_ping;
//        }

    },nullptr,H4P_TRID_IPPD);
    h4pcConnected();
}

void H4P_IPPresence::stop(){
    Serial.printf("H4P_IPPresence::stop\n");
    h4.cancel(_pinger);
    h4pcDisconnected();
}

void H4P_IPPresence::_ping_recv_cb(void *arg, void *pdata){
    Serial.printf("_ping_recv_cb this=%08x @_ping=%08x\n",this,&_ping);
    uint32_t i=1+(reinterpret_cast<struct ping_resp*>(arg)->ping_err);
    Serial.printf("postfacto ping =  %d\n",i);
}
#endif