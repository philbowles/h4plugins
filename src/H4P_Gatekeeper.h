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
#pragma once

#ifdef ARDUINO_ARCH_ESP8266

#include<H4Service.h>
#include<H4P_WiFi.h>

class h4pRoamer;

using H4P_ROAMER_MAP        = vector<h4pRoamer*>;

extern "C" {  
    #include <ping.h>
}
//
// H4P_Gatekeeper
//
class H4P_Gatekeeper: public H4Service{
        static  H4_TIMER                    _chunker;
        static  H4P_ROAMER_MAP::iterator    _matched;
        static  struct  ping_option         _pop;

        static  void            _ping_recv_cb(void *opt, void *resp);
        static  void            _scavenge();
    public:
            H4P_Gatekeeper(): H4Service("gate"){ 
                depend<H4P_WiFi>(wifiTag());
                memset(&_pop,'\0',sizeof(ping_option));
                _pop.count = 1;    //  try to ping how many times
                _pop.coarse_time = H4P_GK_STAGGER;  // ping interval
                ping_regist_recv(&_pop,_ping_recv_cb);
                ping_regist_sent(&_pop,NULL);
            }
#if H4P_LOG_MESSAGES
                void                info() override;
#endif
// syscall only
        virtual void                svcUp() override;
        virtual void                svcDown() override;
};
//
// h4pRoamer
//
class h4pRoamer {
    public:
            string      _ip;
            string      _id;
            string      _name;

        h4pRoamer(const string& name,const string& id);
//syscall
        virtual void        _announce(const string& ip);
        virtual string      _describe(){ return string(_name)+" "+_type()+" "+_id+" "+_ip.data(); }
        virtual void        _startSniffing(){};
        virtual void        _stopSniffing(){};
        virtual string      _type()=0;

        virtual string      getIP(){ return _ip; }
};
//
//      IP
//
class h4pRoamingIP: public h4pRoamer{
    public:
        h4pRoamingIP(const string& name,const string& id);
        h4pRoamingIP(const string& name,const IPAddress& ip);

        virtual string  getIP() override { return _id; }
        virtual string  _type() override { return uppercase(ipTag()); }
};
//
//      MDNS (.local)
//
class h4pRoamingDotLocal: public h4pRoamer{
        string      _service;
        string      _protocol;

        MDNSResponder::hMDNSServiceQuery hsq = 0;
    public:
        static unordered_map<string,h4pRoamingDotLocal*> localList;
        h4pRoamingDotLocal(const string& name,const string& service,const string& protocol);
//      syscall
        virtual string  _describe(){ return string(_name)+" "+_type()+" _"+_service+"._"+_protocol+".local "+_ip.data(); } 
        virtual void    _startSniffing() override;
        virtual void    _stopSniffing() override;
        virtual string  _type() override { return "MDNS"; }
};
//
//      UPNP
//
class h4pRoamingUPNP: public h4pRoamer{
        string          _tag;
    public:
        h4pRoamingUPNP(const string& name,const string& tag,const string& id);
//      syscall
        virtual string  _describe(){ return string(_name)+" "+_type()+" "+_tag+" "+_id+" "+_ip.data(); } 
        virtual void    _startSniffing() override;
        virtual string  _type() override { return uppercase(ipTag()); }
};
//
//      H4
//
class h4pRoamingH4: public h4pRoamingUPNP{
    public:
        h4pRoamingH4(const string& name): h4pRoamingUPNP(name,"X-H4-Device",name){}
        virtual string  _describe(){ return string(_name)+" "+_type()+" "+_ip.data(); } 
        virtual string _type() override { return uppercase(h4Tag()); }
};

#endif