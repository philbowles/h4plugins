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

#include<H4Service.h>
#include<H4P_WiFi.h>
#include<H4P_UPNPServer.h>

class h4pRoamer;

using H4P_ROAMER_MAP = vector<h4pRoamer*>;
extern H4P_ROAMER_MAP       h4pRoamers;

using H4P_GRID_MAP   = std::unordered_map<string,string>;
//
// H4P_Gatekeeper
//
class H4P_Gatekeeper: public H4Service{
    public:
            H4P_Gatekeeper(): H4Service("gate"){ depend<H4P_WiFi>(wifiTag()); }

#if H4P_LOG_MESSAGES
                void        info() override;
#endif
// syscall only
        virtual void        svcUp() override;
        virtual void        svcDown() override;
};
/////////////////////////////////////////////////////////////////
//
// h4pRoamer
//
class h4pRoamer {
    protected:
            string      _name;
            string      _id;
            bool        _present=false;
    public:        
        virtual string      _type(){ return"roam"; }
        h4pRoamer(const string& name,const string& id): _name(name),_id(id){
            require<H4P_Gatekeeper>("gate");
            h4pRoamers.push_back(this);
        }
//syscall
        virtual void    _announce(const string& ip);
        virtual string  _describe(){ return string(_name)+" "+_type()+" "+_id+(_present ? " PRE":" AB")+"SENT "; } 
        virtual void    _startSniffing()=0;
        virtual void    _stopSniffing(){};
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Roamers
//
#ifdef ARDUINO_ARCH_ESP8266
extern "C" {  
    #include <ping.h>
}
class h4pRoamingIP: public h4pRoamer{
        H4_TIMER            _pinger;
        struct ping_option  _pop;
        
               void    _commonCTOR();
        static void    _ping_recv_cb(void *opt, void *resp);
   public:
        virtual string _type() override { return uppercase(ipTag()); }
        h4pRoamingIP(const string& name,const string& id);
        h4pRoamingIP(const string& name,const IPAddress& ip);
//      syscall
        virtual void    _startSniffing() override;
        virtual void    _stopSniffing() override { h4.cancel(_pinger); }
};

class h4pRoamingDotLocal: public h4pRoamer{
        string      _service;
        string      _protocol;

        MDNSResponder::hMDNSServiceQuery hsq = 0;
    public:
        static unordered_map<string,h4pRoamingDotLocal*> localList;
        virtual string _type() override { return "MDNS"; }
        h4pRoamingDotLocal(const string& name,const string& service,const string& protocol);
//      syscall
        virtual string  _describe(){ return string(_name)+" "+_type()+" _"+_service+"._"+_protocol+".local "+(_present ? "PRE":"AB")+"SENT "; } 
        virtual void    _startSniffing() override;
        virtual void    _stopSniffing() override;
};

#endif
class h4pRoamingUPNP: public h4pRoamer{
        H4P_UPNPServer* _pUPNP;
        string          _tag;
    public:
        virtual string _type() override { return "UPNP"; }
        h4pRoamingUPNP(const string& name,const string& tag,const string& id);
//      syscall
        virtual string  _describe(){ return string(_name)+" "+_type()+" "+_tag+" "+_id+" "+(_present ? "PRE":"AB")+"SENT "; } 
        virtual void    _startSniffing() override;
};