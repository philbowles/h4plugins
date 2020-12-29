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

void H4P_UPNPServer::_hookIn(){
    DEPEND(asws);
    REQUIREBT;
    if(!h4wifi._getPersistentValue(nameTag(),"upnp ")) if(_name!="") _cb[nameTag()]=_name;
    H4EVENT("UPNP name %s",CSTR(_cb[nameTag()]));
}

void  H4P_UPNPServer::friendlyName(const string& name){ h4wifi._setPersistentValue(nameTag(),name,true); }

uint32_t H4P_UPNPServer::_friendly(vector<string> vs){
    return guard1(vs,[this](vector<string> vs){
        h4wifi._setPersistentValue(nameTag(),H4PAYLOAD,true);
        return H4_CMD_OK;
    });
}

void H4P_UPNPServer::__upnpSend(uint32_t mx,const string s,IPAddress ip,uint16_t port){
	h4.nTimesRandom(H4P_UDP_REPEAT,0,mx,bind([this](IPAddress ip,uint16_t port,string s) {
		_udp.writeTo((uint8_t *)CSTR(s), s.size(), ip, port);
		},ip,port,s),nullptr,H4P_TRID_UDPS); // name this!!
}

void H4P_UPNPServer::_listenUDP(){
    if(_udp.listenMulticast(_ubIP, 1900)) {
        _udp.onPacket([this](AsyncUDPPacket packet) {
            h4.queueFunction(bind([this](string msg, IPAddress ip, uint16_t port) {
                if(msg.size() > 50){
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
                    uint32_t mx=1000 * atoi(CSTR(replaceAll(uhdrs["CACHE-CONTROL"],"max-age=","")));
                    if (msg[0] == 'M') { //-SEARCH
                        string ST = uhdrs["ST"];
                        if (ST==_pups[1]) { // make tag
                            string tail=((ST==_pups[1]) ? ST:"");
                            __upnpSend(mx, "HTTP/1.1 200 OK\r\nST:" + ST +"\r\n" +__upnpCommon(tail), ip,port);
                        }
                    }
                    else {
                        if (msg[0] == 'N') { //OTIFY
                            string usn=uhdrs["USN"];
                            if(_detect.count(usn)) _detect[usn](mx,uhdrs);
                        }
                    } 
                }
            },stringFromBuff(packet.data(), packet.length()),packet.remoteIP(), packet.remotePort()),nullptr, H4P_TRID_UDPM);
        });
    }
}


string H4P_UPNPServer::__makeUSN(const string& s){
	string full=_uuid+_cb["udn"];
	return s.size() ? full+="::"+s:full;
}

string H4P_UPNPServer::__upnpCommon(const string& usn){
	_cb["usn"]=__makeUSN(usn);
	string rv=replaceParams(_ucom);
	return rv+"\r\n";
}

void H4P_UPNPServer::_start(){
    if(!(WiFi.getMode() & WIFI_AP)){
        _cb["age"]=stringFromInt(H4P_UDP_REFRESH/1000); // fix
        _cb["udn"]="Socket-1_0-upnp"+_cb[chipTag()];
        _cb["updt"]=_pups[2];
        _cb["umfr"]="Belkin International Inc.";
        _cb["usvc"]=_pups[3];
        _cb["usid"]=_urn+"serviceId:basicevent1";

        _xml=replaceParamsFile("/up.xml");
        _ucom=replaceParamsFile("/ucom.txt");
        _soap=H4P_SerialCmd::read("/soap.xml");
// erase redundant _cb?
        _cb.erase("age");
        _cb.erase("updt");
        _cb.erase("umfr");
        _cb.erase("usvc");
        _cb.erase("usid");
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
        _notify(aliveTag()); // TAG
        h4.every(H4P_UDP_REFRESH / 3,[this](){ _notify(aliveTag()); },nullptr,H4P_TRID_NTFY,true); // TAG
        _upHooks();
    }
}

void H4P_UPNPServer::_upnp(AsyncWebServerRequest *request){ // redo
  h4.queueFunction(bind([this](AsyncWebServerRequest *request) {
        string soap=stringFromBuff((const byte*) request->_tempObject,strlen((const char*) request->_tempObject));
        _cb["gs"]=(soap.find("Get")==string::npos) ? "Set":"Get";
        uint32_t _set=soap.find(">1<")==string::npos ? 0:1;
#ifdef H4P_LOG_EVENTS
        if(_cb["gs"]=="Set") _btp->_turn(_set,_pName);
#else
        if(_cb["gs"]=="Set") _btp->turn(_set);
#endif
//        _cb[stateTag()]=stringFromInt(_btp->state());
        request->send(200, "text/xml", CSTR(replaceParams(_soap))); // refac
    },request),nullptr, H4P_TRID_SOAP);
}

void H4P_UPNPServer::_stop(){
    _notify("byebye");
    h4.cancelSingleton(H4P_TRID_NTFY);
    _udp.close();
    _downHooks();
}

void H4P_UPNPServer::_notify(const string& phase){ // h4Chunker it up
    h4Chunker<vector<string>>(_pups,[this,phase](vector<string>::const_iterator i){ 
        string NT=(*i).size() ? (*i):__makeUSN("");
        string nfy="NOTIFY * HTTP/1.1\r\nHOST:"+string(_ubIP.toString().c_str())+":1900\r\nNTS:ssdp:"+phase+"\r\nNT:"+NT+"\r\n"+__upnpCommon((*i));
        broadcast(H4P_UDP_JITTER,CSTR(nfy));
    });
}

string H4P_UPNPServer::replaceParams(const string& s){ // oh for a working regex!
	int i=0;
	int j=0;
	string rv(s);

	while((i=rv.find("%",i))!=string::npos){
        if(j){
            string var=rv.substr(j+1,i-j-1);
            if(_cb.count(var)) {
                rv.replace(j,i-j+1,_cb[var]); // FIX!!
                rv.shrink_to_fit();
            }
            j=0;
        } else j=i;    
        ++i;
	}
	return rv.c_str();	
}