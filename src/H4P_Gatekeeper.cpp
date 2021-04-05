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
#include<H4P_Gatekeeper.h>
#include<H4P_UPNPServer.h>

unordered_map<string,h4pRoamingDotLocal*> h4pRoamingDotLocal::localList;

void h4pMDNScb(MDNSResponder::MDNSServiceInfo serviceInfo, MDNSResponder::AnswerType answerType, bool p_bSetContent) {
    if(answerType ==  MDNSResponder::AnswerType::IP4Address){
        vector<string> vs=split(string(serviceInfo.serviceDomain()),".");
        string who=vs[0];
        string ip=p_bSetContent ? serviceInfo.IP4Adresses()[0].toString().c_str():"";
        if(h4pRoamingDotLocal::localList.count(who)){ h4pRoamingDotLocal::localList[who]->_announce(ip); }
    }
}
//
//      Gatekeeper
//
#if H4P_LOG_MESSAGES
void H4P_Gatekeeper::info() { 
    H4Service::info();
    if(h4pRoamers.size()){
        reply(" Roamers:");
        for(auto const& r:h4pRoamers) reply("  %s",CSTR(r->_describe()));
    }
}
#endif

void H4P_Gatekeeper::svcUp(){
    h4pRoamers.shrink_to_fit();
    for(auto const& r:h4pRoamers) r->_startSniffing();
    H4Service::svcUp();
}

void H4P_Gatekeeper::svcDown(){
    for(auto const& r:h4pRoamers) r->_stopSniffing();
    H4Service::svcDown();
}
//
//      roamers
//
extern H4P_ROAMER_MAP          h4pRoamers;

void  h4pRoamer::_announce(const string& ip){
    bool b=ip.size();
    if(b ^ _present){
        _present=b;
        h4psysevent(_name,H4PE_PRESENCE,"%s",CSTR(ip));
    }
}
//
//      h4pRoamingIP
//
#ifdef ARDUINO_ARCH_ESP8266

h4pRoamingIP::h4pRoamingIP(const string& name,const string& id): h4pRoamer(name,id){ _commonCTOR(); }

h4pRoamingIP::h4pRoamingIP(const string& name,const IPAddress& ip): h4pRoamer(name,"unset"){
    _id=ip.toString().c_str();
    _commonCTOR();
}

void h4pRoamingIP::_commonCTOR(){
    memset(&_pop,'\0',sizeof(ping_option));
    _pop.count = 1;    //  try to ping how many times
    _pop.coarse_time = 1;  // ping interval
    _pop.ip = ipaddr_addr(_id.c_str());
    _pop.reverse=this;
    ping_regist_recv(&_pop,_ping_recv_cb);
    ping_regist_sent(&_pop,NULL);
}

void h4pRoamingIP::_ping_recv_cb(void *opt, void *resp){
    auto pOpt=static_cast<struct ping_option*>(opt);
    auto caller=reinterpret_cast<h4pRoamingIP*>(pOpt->reverse);
    int v=(1+static_cast<struct ping_resp*>(resp)->ping_err);
    IPAddress ip(pOpt->ip);
    h4.queueFunction([=](){ caller->_announce(v ? ip.toString().c_str():""); });
}

void h4pRoamingIP::_startSniffing(){
    static uint32_t stagger=0;
    uint32_t stag=H4P_IPPD_RATE / (1+h4pRoamers.size());
    h4.once(stagger,[=]{ _pinger=h4.everyRandom(H4P_PJ_LO,H4P_PJ_HI,[=](){ ping_start(&_pop); },nullptr,H4P_TRID_IPPD); });
    stagger+=stag;
}
//
//      MDNS
//
h4pRoamingDotLocal::h4pRoamingDotLocal(const string& name,const string& service,const string& protocol):
    _service(service),
    _protocol(protocol),
    h4pRoamer(name,name){ 
        localList[name]=this;
}

void h4pRoamingDotLocal::_startSniffing(){
    if(!hsq) hsq=MDNS.installServiceQuery(CSTR(_service),CSTR(_protocol), h4pMDNScb);
}

void h4pRoamingDotLocal::_stopSniffing(){ 
    MDNS.removeServiceQuery(hsq); 
    hsq=0;
}
#endif
//
//      UPNP
//
h4pRoamingUPNP::h4pRoamingUPNP(const string& name,const string& tag,const string& id): _tag(tag), h4pRoamer(name,id){ 
    _pUPNP=require<H4P_UPNPServer>(upnpTag());
    h4pregisterhandler("uprh",H4PE_UPNP,[=](const string& svc,H4PE_TYPE t,const string& msg){
        auto vs=split(msg,",");
        if(vs[1]==_tag) _announce(vs[0]);
    });
}

void h4pRoamingUPNP::_startSniffing(){ _pUPNP->_listenTag(_tag,_id); }