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

#include<H4P_UPNPServer.h>
#include<H4P_SerialCmd.h>

using H4P_UPNP_TAGMAP = unordered_map<string,unordered_set<string>>;
H4P_UPNP_TAGMAP h4pUPNPMap;

string H4P_UPNPServer::__makeUSN(const string& s){
	string full=_uuid+_udn;
	return s.size() ? full+="::"+s:full;
}

string H4P_UPNPServer::__upnpCommon(const string& usn){
	h4p.gvSetstring("usn",__makeUSN(usn));
	string rv=h4preplaceparams(_ucom);
	return rv+"\r\n";
}

void H4P_UPNPServer::__upnpSend(uint32_t mx,const string s,IPAddress ip,uint16_t port){
	h4.nTimesRandom(H4P_UDP_REPEAT,0,mx,[=]() {
		_udp.writeTo((uint8_t *)CSTR(s), s.size(), ip, port);
	},nullptr,H4P_TRID_UDPS); // name this!!
}

void H4P_UPNPServer::_handleEvent(const string& svc,H4PE_TYPE t,const string& msg){
    switch(t){
        case H4PE_VIEWERS:
            {
                uint32_t mode=STOI(msg);
                if(mode) {
                #if H4P_USE_WIFI_AP
                    if(mode==WIFI_AP) h4puiAdd(nameTag(),H4P_UI_INPUT,"s");
                    else h4puiAdd(nameTag(),H4P_UI_TEXT,"s");
                #else
                    h4puiAdd(nameTag(),H4P_UI_TEXT,"s");
                #endif
                }
            }
            break;
        case H4PE_GVCHANGE:
            if(_running && svc==nameTag()){
//                h4puiSync(nameTag(),msg);
                svcDown(); // shut down old name, send bye bye etc
                svcUp();
            }
    }
}

void H4P_UPNPServer::_handlePacket(string p,IPAddress ip,uint16_t port){
    H4P_NVP_MAP uhdrs;
    vector<string> hdrs = split(p, "\r\n");
    while (hdrs.back() == "") hdrs.pop_back();
    if(hdrs.size() > 4){
        for (auto const &h :vector<string>(++hdrs.begin(), hdrs.end())) {
            vector<string> parts=split(h,":");
            if(parts.size()) uhdrs[parts[0]]=ltrim(join(vector<string>(++parts.begin(),parts.end()),":"));
        }
    }
    uint32_t mx=1000 * atoi(CSTR(replaceAll(uhdrs["CACHE-CONTROL"],"max-age=","")));
    switch(p[0]){
        case 'M':
            {
                string ST = uhdrs["ST"];
                if (ST==_pups[1]) { // make tag
                    string tail=((ST==_pups[1]) ? ST:"");
                    __upnpSend(mx, "HTTP/1.1 200 OK\r\nST:" + ST +"\r\n" +__upnpCommon(tail), ip,port);
                }
            }
            break;
        case 'N':
            for(auto &L:h4pUPNPMap){
                if(uhdrs.count(L.first)){
                    string v=uhdrs[L.first];
                    if(L.second.count("*") || L.second.count(v)) {
                        h4psysevent(v,H4PE_UPNP,"%s,%s",(replaceAll(uhdrs["NTS"],"ssdp:","")==aliveTag()) ? CSTR(ip.toString()):"",CSTR(L.first));
                    }
                }
            }
    }
}

void H4P_UPNPServer::_init(){
    _pups.push_back(_urn+"device:controllee:1");
    _pups.push_back(_urn+"service:basicevent:1");

    string dn=string(uppercase(h4Tag())+" "+deviceTag()+" ");

    if(h4p[nameTag()]=="") h4p[nameTag()]=dn.append(h4p[chipTag()]);
    h4p.gvSave(nameTag());
    XLOG("UPNP name %s",CSTR(h4p[nameTag()]));
}

void H4P_UPNPServer::_listenTag(const string& tag,const string& value){ h4pUPNPMap[tag].insert(value); }

void H4P_UPNPServer::_listenUDP(){ 
    if(!_udp.listenMulticast(_ubIP, 1900)) return; // some kinda error?
    _udp.onPacket([this](AsyncUDPPacket packet){
        string pkt((const char*)packet.data(),packet.length());
        IPAddress ip=packet.remoteIP();
        uint16_t port=packet.remotePort();
        h4.queueFunction([=](){ _handlePacket(pkt,ip,port); }); // shud be named etc
    }); 
}

void H4P_UPNPServer::_notify(const string& phase){
    h4Chunker<vector<string>>(_pups,[=](vector<string>::iterator i){ 
        string NT=(*i).size() ? (*i):__makeUSN("");
        string nfy="NOTIFY * HTTP/1.1\r\nHOST:"+string(_ubIP.toString().c_str())+":1900\r\nNTS:ssdp:"+phase+"\r\nNT:"+NT+"\r\n"+__upnpCommon((*i));
        _broadcast(H4P_UDP_JITTER,CSTR(nfy));
    },H4_JITTER_LO,H4_JITTER_HI,[=]{ h4p.gvErase("usn"); });
}

void H4P_UPNPServer::_upnp(AsyncWebServerRequest *request){ // redo
    h4.queueFunction([=]() {
        string soap((const char*) request->_tempObject,strlen((const char*) request->_tempObject));
       	h4p.gvSetstring("gs",(soap.find("Get")==string::npos) ? "Set":"Get"); 
        uint32_t _set=soap.find(">1<")==string::npos ? 0:1;
        if(h4p["gs"]=="Set") h4p.gvSetInt(stateTag(),_set); //=//XEVENT(H4PE_ONOF,"%d",_set);
        request->send(200, "text/xml", CSTR(h4preplaceparams(_soap))); // refac
        h4p.gvErase("gs");
    },nullptr, H4P_TRID_SOAP);
}

#if H4P_LOG_MESSAGES
void H4P_UPNPServer::info(){ 
    H4Service::info();
    reply(" Name: %s UDN=%s",CSTR(h4p[nameTag()]),CSTR(_udn));
}
#endif

void H4P_UPNPServer::svcDown(){
    h4.cancelSingleton(H4P_TRID_NTFY);
    _notify("byebye");
    _udp.close(); // delay this RW?
    H4Service::svcDown();
}

void H4P_UPNPServer::svcUp(){
#if H4P_USE_WIFI_AP
    if(WiFi.getMode()==WIFI_AP) return;
#endif
    _udn=string("Socket-1_0-upnp").append(h4p[chipTag()]);
    h4p.gvSetstring("udn",_udn);
    h4p.gvSetstring(ageTag(),stringFromInt(H4P_UDP_REFRESH/1000));
    h4p.gvSetstring("updt",_pups[2]);
    h4p.gvSetstring("umfr","Belkin International Inc.");
    h4p.gvSetstring("usvc",_pups[3]);
    h4p.gvSetstring("usid",_urn+"serviceId:basicevent1");

    _xml=replaceParamsFile("/up.xml");
    _ucom=replaceParamsFile("/ucom.txt");
    _soap=H4P_SerialCmd::read("/soap.xml");

    h4p.gvErase({"udn",ageTag(),"updt","umfr","usvc","usid"});
//
    _pWiFi->on("/we",HTTP_GET, [this](AsyncWebServerRequest *request){ request->send(200,"text/xml",CSTR(_xml)); });
    _pWiFi->on("/upnp", HTTP_POST,[this](AsyncWebServerRequest *request){ _upnp(request); },
        NULL,
        [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total){
            if(!index) request->_tempObject = malloc(total+1);
            memcpy((uint8_t*) request->_tempObject+index,data,len);
            if(index + len == total) *((uint8_t*) request->_tempObject+total)='\0';
        }
    );
    _listenUDP();
    _notify(aliveTag()); // TAG
    h4.every(H4P_UDP_REFRESH / 2,[=](){ _notify(aliveTag()); },nullptr,H4P_TRID_NTFY,true); // TAG
    H4Service::svcUp();
}