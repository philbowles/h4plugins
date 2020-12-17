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

#include<H4P_PresenceDetector.h>
#include<H4P_BinaryThing.h>
#include<H4P_UPNPServer.h>
//
//      UPNP
//
void H4P_UPNPDetector::_hookIn() { 
    REQUIRE(onof);
    DEPEND(upnp);
}

void H4P_UPNPDetector::_start(){
    h4upnp._listenUSN(_id,[this](uint32_t mx,H4P_CONFIG_BLOCK uh){
        bool direction=uh["NTS"].find(aliveTag())!=string::npos;
        h4.cancel(_pinger);
        if(direction) _pinger=h4.once(mx,[this](){ _inout(false); },nullptr,H4P_TRID_UDPU);
        _inout(direction);
    });
    _upHooks();
}

H4P_UPNPDetectorSource::H4P_UPNPDetectorSource(const string& pid,const string& id): H4P_UPNPDetector(pid,id){
    H4P_BinaryThing* _btp;
    REQUIREBT;
    if(_btp){
        _f=[this,_btp](bool b){ 
#ifdef H4P_LOG_EVENTS
        _btp->_turn(b,_pName+string("("+_id+")"));
#else
        _btp->turn(b);
#endif
        };
    }
}

#ifdef ARDUINO_ARCH_ESP8266
extern "C" {
  #include <ping.h>
}

bool                H4P_IPDetector::_inflight=false;
struct ping_option  H4P_IPDetector::pop;
//
//      IP
//
void H4P_IPDetector::_hookIn() { DEPEND(wifi); }

void H4P_IPDetector::_start(){
    _pinger=h4.everyRandom(H4P_PJ_LO,H4P_PJ_HI,[this](){
        if(!_inflight){
            _inflight=true;
            memset(&pop,'\0',sizeof(ping_option));
            pop.count = 1;    //  try to ping how many times
            pop.coarse_time = 1;  // ping interval
            pop.ip = ipaddr_addr(_id.c_str());
            pop.reverse=this;
            ping_regist_recv(&pop,reinterpret_cast<ping_recv_function>(&H4P_IPDetector::_ping_recv_cb));
            ping_regist_sent(&pop,NULL);
            ping_start(&pop);
        }
    },nullptr,H4P_TRID_IPPD);
    _upHooks();
} 

void H4P_IPDetector::_ping_recv_cb(void *arg, void *pdata){
    H4P_IPDetector* p=reinterpret_cast<H4P_IPDetector*>(reinterpret_cast<struct ping_option*>(arg)->reverse);
    uint32_t v=1+(reinterpret_cast<struct ping_resp*>(pdata)->ping_err);
    h4.queueFunction(bind([](H4P_IPDetector* p,uint32_t v){ 
        p->_inout(v);
        _inflight=false;
    },p,v));
}
H4P_IPDetectorSource::H4P_IPDetectorSource(const string& pid,const string& id): H4P_IPDetector(pid,id){
    H4P_BinaryThing* _btp;
    REQUIREBT;
    if(_btp){
        _f=[this,_btp](bool b){ 
#ifdef H4P_LOG_EVENTS
        _btp->_turn(b,_pName+string("("+_id+")"));
#else
        _btp->turn(b);
#endif
        };
    }
}
#ifdef H4P_USE_OTA
//
//      MDNS
//
unordered_map<string,H4P_MDNSDetector*> H4P_MDNSDetector::localList;

void H4P_MDNSDetector::_hookIn() { 
    REQUIRE(onof);
    DEPEND(wifi);
}

void H4P_MDNSDetector::_start(){
    MDNS.installServiceQuery(CSTR(_service),CSTR(_protocol), H4P_MDNSDetector::MDNSServiceQueryCallback);
    _upHooks();
}

H4P_MDNSDetector::H4P_MDNSDetector(const string& friendly,const string& service,const string& protocol,H4BS_FN_SWITCH f):
    _service(service),
    _protocol(protocol),
    H4PDetector(friendly,friendly,f){ 
        localList[friendly]=this;
}

H4P_H4DetectorSource::H4P_H4DetectorSource(const string& id): H4P_H4Detector(id){
    H4P_BinaryThing* _btp;
    REQUIREBT;
    if(_btp){
        _f=[this,_btp](bool b){ 
#ifdef H4P_LOG_EVENTS
        _btp->_turn(b,_pName+string("("+_id+")"));
#else
        _btp->turn(b);
#endif
        };
    }
}
#endif // OTA
#endif // 8266