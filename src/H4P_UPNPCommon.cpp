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
#include<H4P_UPNPCommon.h>
#ifndef H4P_NO_WIFI

void H4P_UPNPCommon::_pseudoHookIn(){
    if(H4Plugin::isLoaded(aswsTag())){
        _ubIP=IPAddress(239,255,255,250);

        h4._hookLoop(nullptr,{ 
            {H4P_TRID_SOAP, "SOAP"},
            {H4P_TRID_UDPM, "UDPM"},
            {H4P_TRID_UDPS, "UDPS"},
            {H4P_TRID_NTFY, "NTFY"} 
            },H4PC_UPNP);

        h4sc.addCmd(upnpTag(),H4PC_ROOT, H4PC_UPNP, nullptr);
        h4sc.addCmd(nameTag(),H4PC_UPNP,0, CMDVS(_friendly));

        h4asws.hookConnect([this](){ start(); });
        h4asws.hookDisconnect([this](){ stop(); });
        H4PluginService::hookFactory([this](){ SPIFFS.remove(CSTR(string("/"+string(nameTag())))); });
    } else { 
        #ifdef H4_LOG_EVENTS
            //DEPENDFAIL(asws);
            h4sc.logEventType(H4P_LOG_DEPENDFAIL,"%s->%s", H4PC_UPNP,aswsTag());
        #endif
    }
}

void  H4P_UPNPCommon::friendlyName(const string& name){ h4wifi.setPersistentValue(nameTag(),name,true); }

uint32_t H4P_UPNPCommon::_friendly(vector<string> vs){
    return H4Plugin::guard1(vs,[this](vector<string> vs){
        h4wifi.setPersistentValue(nameTag(),PAYLOAD,true);
        return H4_CMD_OK;
    });
}

void H4P_UPNPCommon::__upnpSend(uint32_t mx,const string s,IPAddress ip,uint16_t port){
	h4.nTimesRandom(H4P_UDP_REPEAT,0,mx,bind([this](IPAddress ip,uint16_t port,string s) {
		_udp.writeTo((uint8_t *)CSTR(s), s.size(), ip, port);
		},ip,port,s),nullptr,H4P_TRID_UDPS); // name this!!
}

void H4P_UPNPCommon::_listenUDP(){
if(_udp.listenMulticast(_ubIP, 1900)) {
    _udp.onPacket([this](AsyncUDPPacket packet) {
        h4.queueFunction(bind([this](string msg, IPAddress ip, uint16_t port) {
            if (msg[0] == 'M') {
                H4P_CONFIG_BLOCK uhdrs;
                vector<string> hdrs = split(msg, "\r\n");
                while (hdrs.back() == "") hdrs.pop_back();
                for (auto const &h :vector<string>(++hdrs.begin(), hdrs.end())) {
                        size_t p = h.find(":");
                        if (p != string::npos) {
                            string key=uppercase(string(h.begin(),h.begin()+p));
                            uhdrs[key]=trim(string(h.begin()+p+1,h.end()));
                        }
                }
                string ST = uhdrs["ST"];
                if (ST==_pups[1] || ST==_uuid+H4Plugin::_cb["udn"]) { // make tag
                    string tail=((ST==_pups[1]) ? ST:"");
                    __upnpSend(1000 * atoi(CSTR(uhdrs["MX"])), "HTTP/1.1 200 OK\r\nST:" + ST +"\r\n" +__upnpCommon(tail), ip,port);
                }
            }
        },stringFromBuff(packet.data(), packet.length()),packet.remoteIP(), packet.remotePort()),nullptr, H4P_TRID_UDPM);
    });
   }
}

string H4P_UPNPCommon::__makeUSN(const string& s){
	string full=_uuid+H4Plugin::_cb["udn"];
	return s.size() ? full+="::"+s:full;
}

string H4P_UPNPCommon::__upnpCommon(const string& usn){
	H4Plugin::_cb["usn"]=__makeUSN(usn);
	string rv=H4P_WiFi::replaceParams(_ucom);
	return rv+"\r\n\r\n";
}

void H4P_UPNPCommon::start(){
    H4Plugin::_cb[nameTag()]=_name;
    h4wifi.getPersistentValue(nameTag(),"upnp ");
    if(!(WiFi.getMode() & WIFI_AP)){
        H4Plugin::_cb["age"]=stringFromInt(H4P_UDP_REFRESH/1000); // fix

        H4Plugin::_cb["udn"]="Socket-1_0-upnp"+H4Plugin::_cb[chipTag()];
        H4Plugin::_cb["updt"]=_pups[2];
        H4Plugin::_cb["umfr"]="Belkin International Inc.";
        H4Plugin::_cb["usvc"]=_pups[3];
        H4Plugin::_cb["usid"]=_urn+"serviceId:basicevent1";

        _xml=H4P_WiFi::replaceParamsFile("/up.xml");
        _ucom=H4P_WiFi::replaceParamsFile("/ucom.txt");
        _soap=H4P_SerialCmd::read("/soap.xml");
// erase redundant H4Plugin::_cb?
        H4Plugin::_cb.erase("age");
        H4Plugin::_cb.erase("updt");
        H4Plugin::_cb.erase("umfr");
        H4Plugin::_cb.erase("usvc");
        H4Plugin::_cb.erase("usid");
//
        h4asws.on("/we",HTTP_GET, [this](AsyncWebServerRequest *request){ request->send(200,"text/xml",CSTR(_xml)); });
        h4asws.on("/upnp", HTTP_POST,[this](AsyncWebServerRequest *request){ _upnp(request); },
            NULL,
            [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total){
                if(!index) request->_tempObject = malloc(total+1);
                memcpy((uint8_t*) request->_tempObject+index,data,len);
                if(index + len == total) *((uint8_t*) request->_tempObject+total)='\0';
            }
        );
        _listenUDP();
        _notify("alive"); // TAG
        h4.every(H4P_UDP_REFRESH / 3,[this](){ _notify("alive"); },nullptr,H4P_TRID_NTFY,true); // TAG
        H4PluginService::svc(upnpTag(),H4P_LOG_SVC_UP); // simulate service
    }
}

void H4P_UPNPCommon::_upnp(AsyncWebServerRequest *request){ // redo
  h4.queueFunction(bind([this](AsyncWebServerRequest *request) {
        string soap=stringFromBuff((const byte*) request->_tempObject,strlen((const char*) request->_tempObject));
        H4Plugin::_cb["gs"]=(soap.find("Get")==string::npos) ? "Set":"Get";
        uint32_t _set=soap.find(">1<")==string::npos ? 0:1;
        if(H4Plugin::_cb["gs"]=="Set"){
            _turn(_set,upnpTag());//
        }
        H4Plugin::_cb[stateTag()]=stringFromInt(_getState());
        request->send(200, "text/xml", CSTR(H4P_WiFi::replaceParams(_soap))); // refac
    },request),nullptr, H4P_TRID_SOAP); // TRID_SOAP
}

void H4P_UPNPCommon::stop(){
    _notify("byebye");
    h4.cancelSingleton(H4P_TRID_NTFY);
    _udp.close();
    H4PluginService::svc(upnpTag(),H4P_LOG_SVC_DOWN); // simulate service
}

void H4P_UPNPCommon::_notify(const string& phase){ // chunker it up
    chunker<vector<string>>(_pups,[this,phase](vector<string>::const_iterator i){ 
        string NT=(*i).size() ? (*i):__makeUSN("");
        string nfy="NOTIFY * HTTP/1.1\r\nHOST:"+string(_ubIP.toString().c_str())+":1900\r\nNTS:ssdp:"+phase+"\r\nNT:"+NT+"\r\n"+__upnpCommon((*i));
        broadcast(H4P_UDP_JITTER,CSTR(nfy));
    });
}
#endif