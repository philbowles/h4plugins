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
#ifdef ARDUINO_ARCH_ESP8266

#include<H4P_Gatekeeper.h>
#include<H4P_UPNPServer.h>

H4_TIMER                    H4P_Gatekeeper::_chunker=nullptr;
H4P_ROAMER_MAP::iterator    H4P_Gatekeeper::_matched;
struct  ping_option         H4P_Gatekeeper::_pop;

unordered_map<string,h4pRoamingDotLocal*> h4pRoamingDotLocal::localList;

void h4pMDNScb(MDNSResponder::MDNSServiceInfo serviceInfo, MDNSResponder::AnswerType answerType, bool p_bSetContent) {
    if(answerType ==  MDNSResponder::AnswerType::IP4Address){
        vector<string> vs=split(string(serviceInfo.serviceDomain()),".");
        string who=vs[0];
        string ip=p_bSetContent ? serviceInfo.IP4Adresses()[0].toString().c_str():"";
        if(h4pRoamingDotLocal::localList.count(who)){ h4.queueFunction([=]{ h4pRoamingDotLocal::localList[who]->_announce(ip); }); }
    }
}
//
//      Gatekeeper
//
void H4P_Gatekeeper::_ping_recv_cb(void *opt, void *resp){
    auto pOpt=static_cast<struct ping_option*>(opt);
    auto caller=reinterpret_cast<h4pRoamer*>(pOpt->reverse);
    int v=(1+static_cast<struct ping_resp*>(resp)->ping_err);
    IPAddress ip(pOpt->ip);
    h4.queueFunction([=](){ caller->_announce(v ? ip.toString().c_str():""); });
    _matched++;
}

void H4P_Gatekeeper::_scavenge() {
    _matched=h4pRoamers.begin();
    h4.cancel(_chunker);
    _chunker=h4.repeatWhile(
        []{ return _matched!=h4pRoamers.end(); },
        (H4P_GATEKEEPER_STAGGER * 3000) / 2, // 1.5 * ping t/o
        []{
            if(_matched!=h4pRoamers.end()){
                auto p=*_matched;
                string mip=p->getIP();
                if(mip.size()){
                    _pop.ip = ipaddr_addr(mip.data());
                    _pop.reverse=p;
                    ping_start(&_pop);
                } 
                else _matched++;
            }
        }
    );
}

#if H4P_LOG_MESSAGES
void H4P_Gatekeeper::info() { 
    H4Service::info();
    if(h4pRoamers.size()){
        reply(" Roamers:");
        for(auto const& r:h4pRoamers) reply("  %s ",CSTR(r->_describe()));
    }
}
#endif

void H4P_Gatekeeper::svcUp(){
    h4pRoamers.shrink_to_fit();
    for(auto const& r:h4pRoamers) r->_startSniffing();
    h4.every(H4P_GATEKEEPER_SCAVENGE,_scavenge,nullptr,H4P_TRID_GATE,true);
    H4Service::svcUp();
}

void H4P_Gatekeeper::svcDown(){
    h4.cancelSingleton(H4P_TRID_GATE);
    h4.cancel(_chunker);
    for(auto &r:h4pRoamers) {
        r->_stopSniffing();
        r->_ip="";
    }
    H4Service::svcDown();
}
//
//      roamers
//
extern H4P_ROAMER_MAP          h4pRoamers;

void  h4pRoamer::_announce(const string& ip){
    if(ip!=_ip){
        Serial.printf("%s _announce ip CHANGE %s to %s\n",_name.data(),_ip.data(),ip.data());
        _ip=ip;
        h4psysevent(_name,H4PE_PRESENCE,"%s",_ip.data());
    }
}
//
//      IP
//
h4pRoamingIP::h4pRoamingIP(const string& name,const string& id): h4pRoamer(name,id){}

h4pRoamingIP::h4pRoamingIP(const string& name,const IPAddress& ip): h4pRoamer(name,ip.toString().c_str()){}
//
//      MDNS
//
h4pRoamingDotLocal::h4pRoamingDotLocal(const string& name,const string& service,const string& protocol):
    _service(service),
    _protocol(protocol),
    h4pRoamer(name,name){ 
        localList[name]=this;
}

void h4pRoamingDotLocal::_startSniffing(){ if(!hsq) hsq=MDNS.installServiceQuery(CSTR(_service),CSTR(_protocol), h4pMDNScb); }

void h4pRoamingDotLocal::_stopSniffing(){ 
    MDNS.removeServiceQuery(hsq); 
    hsq=0;
}
//
//      UPNP
//
h4pRoamingUPNP::h4pRoamingUPNP(const string& name,const string& tag,const string& id): _tag(tag), h4pRoamer(name,id){ 
    require<H4P_UPNPServer>(upnpTag());
    h4pregisterhandler(name,H4PE_UPNP,[=](const string& svc,H4PE_TYPE t,const string& msg){ if(svc==_id) _announce(msg); });
}

void h4pRoamingUPNP::_startSniffing(){ H4P_UPNPServer::_listenTag(_tag,_id); }
#endif