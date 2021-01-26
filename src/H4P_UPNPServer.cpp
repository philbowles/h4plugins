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
/*
Received 26/01/2021 at 14:39:05 (519)

M-SEARCH * HTTP/1.1
ST: upnp:rootdevice
MAN: "ssdp:discover"
HOST: [ff02::c]:1900
MX: 10
Content-Length: 0

*/

void H4P_UPNPServer::_handleEvent(H4PID pid,H4P_EVENT_TYPE type,const string &msg){ 
    _pWiFi->_wipe(nameTag());
//    _notify("byebye");
//    delay(1000); // naughty but nice :) 
}

void H4P_UPNPServer::_hookIn(){
    _btp=h4prequire<H4P_BinaryThing>(H4PID_ONOF);
    _pWiFi=h4pdepend<H4P_WiFi>(this,H4PID_WIFI);
    string dn=string(h4Tag())+" "+deviceTag()+" ";
    if(!_pWiFi->_getPersistentValue(nameTag(),dn)) if(_name!="") _cb[nameTag()]=_name;
    PLOG("UPNP name %s",CSTR(_cb[nameTag()]));
    _pWiFi->_uiAdd(H4P_UIO_NAME,"name",H4P_UI_LABEL,"",[]{ return _cb[nameTag()]; }); // cos we don't know it yet
    _listenTag("X-H4-DEVICE","*",[this](uint32_t && mx,H4P_CONFIG_BLOCK && uh,bool && direction){
        if(uh["NT"]=="upnp:rootdevice"){
            if(direction) PEVENT(H4P_EVENT_H4_ENTER,"%s,%s,%s,%d",CSTR(uh["X-H4-DEVICE"]),CSTR(uh["X-H4-CHIP"]),CSTR(uh["LOCATION"]),mx); // stag it
            else PEVENT(H4P_EVENT_H4_LEAVE,"%s",CSTR(uh["X-H4-DEVICE"])); // stag it
        }
    });
    H4Plugin::_hookIn();
}

uint32_t H4P_UPNPServer::_host2(vector<string> vs){ return _guardString2(vs,[this](string a,string b){ setBothNames(a,b); return H4_CMD_OK; }); }

void  H4P_UPNPServer::friendlyName(const string& name){ _pWiFi->_setPersistentValue(nameTag(),name,true); }

uint32_t H4P_UPNPServer::_friendly(vector<string> vs){
    return _guard1(vs,[this](vector<string> vs){
        _pWiFi->_setPersistentValue(nameTag(),H4PAYLOAD,true);
        return H4_CMD_OK;
    });
}

void H4P_UPNPServer::__upnpSend(uint32_t mx,const string s,IPAddress ip,uint16_t port){
	h4.nTimesRandom(H4P_UDP_REPEAT,0,mx,[=]() {
		_udp.writeTo((uint8_t *)CSTR(s), s.size(), ip, port);
	},nullptr,H4P_TRID_UDPS); // name this!!
}

void H4P_UPNPServer::_handlePacket(string p,IPAddress ip,uint16_t port){
//    static bool cursed=false;
//    if(cursed) Serial.printf("I BEEN RECURSED!!!!!!!!!!!!!!!!!!!*********************\n");
//    cursed=true;
    if(p.size() > 50){
        H4P_CONFIG_BLOCK uhdrs;
        vector<string> hdrs = split(p, "\r\n");
        while (hdrs.back() == "") hdrs.pop_back();
        if(hdrs.size() > 4){
            for (auto const &h :vector<string>(++hdrs.begin(), hdrs.end())) {
                vector<string> parts=split(h,":");
                if(parts.size()){
                    string key=uppercase(parts[0]);
                    uhdrs[key]=join(vector<string>(++parts.begin(),parts.end()),":");
                } else Serial.printf("NO COLONS %s\n",CSTR(h)); 
            }
        } else Serial.printf("TiNY BLOCK Nh=%d\n",hdrs.size()); 

        uint32_t mx=1000 * atoi(CSTR(replaceAll(uhdrs["CACHE-CONTROL"],"max-age=","")));
        if (p[0] == 'M') { //-SEARCH
            string ST = uhdrs["ST"];
            if (ST==_pups[1]) { // make tag
                string tail=((ST==_pups[1]) ? ST:"");
                __upnpSend(mx, "HTTP/1.1 200 OK\r\nST:" + ST +"\r\n" +__upnpCommon(tail), ip,port);
            }
        }
        else {
            if (p[0] == 'N') { //OTIFY
                for(auto const& d:_detect){
                    string tag=d.first;
                    if(uhdrs.count(tag)){
                        if(d.second.first=="*" || uhdrs[tag].find(d.second.first)!=string::npos){
                            d.second.second(mx,uhdrs,uhdrs["NTS"].find(aliveTag())!=string::npos);
                        }
                    }
                }
            }
        }
    } else Serial.printf("WTF SHORT SHIT %s\n",CSTR(p));
//    cursed=false;
}

void H4P_UPNPServer::_listenUDP(){ 
    if(!_udp.listenMulticast(_ubIP, 1900)) return; // some kinda error?
    _udp.onPacket([this](AsyncUDPPacket packet){
//        Serial.printf("pkt sz=%d\n",packet.length());
        string pkt=stringFromBuff(packet.data(),packet.length());
        IPAddress ip=packet.remoteIP();
        uint16_t port=packet.remotePort();
        h4.queueFunction([this,pkt,ip,port](){ _handlePacket(pkt,ip,port); });
    }); 
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
    h4.every(H4P_UDP_REFRESH / 2,[this](){ _notify(aliveTag()); },nullptr,H4P_TRID_NTFY,true); // TAG
    _upHooks();
}

void H4P_UPNPServer::_upnp(AsyncWebServerRequest *request){ // redo
  h4.queueFunction([=]() {
        string soap=stringFromBuff((const byte*) request->_tempObject,strlen((const char*) request->_tempObject));
        _cb["gs"]=(soap.find("Get")==string::npos) ? "Set":"Get";
        uint32_t _set=soap.find(">1<")==string::npos ? 0:1;
#if H4P_LOG_EVENTS
        if(_cb["gs"]=="Set") _btp->_turn(_set,_pName);
#else
        if(_cb["gs"]=="Set") _btp->turn(_set);
#endif
//        _cb[stateTag()]=stringFromInt(_btp->state());
        request->send(200, "text/xml", CSTR(replaceParams(_soap))); // refac
    },nullptr, H4P_TRID_SOAP);
}

void H4P_UPNPServer::_stop(){
    h4.cancelSingleton(H4P_TRID_NTFY);
    _notify("byebye");
    _udp.close();
    _downHooks();
}

void H4P_UPNPServer::_notify(const string& phase){ // h4Chunker it up
    h4Chunker<vector<string>>(_pups,[this,phase](vector<string>::const_iterator i){ 
        string NT=(*i).size() ? (*i):__makeUSN("");
        string nfy="NOTIFY * HTTP/1.1\r\nHOST:"+string(_ubIP.toString().c_str())+":1900\r\nNTS:ssdp:"+phase+"\r\nNT:"+NT+"\r\n"+__upnpCommon((*i));
        _broadcast(H4P_UDP_JITTER,CSTR(nfy));
    });
}

string H4P_UPNPServer::replaceParams(const string& s){ // oh for a working regex!
//    Serial.printf("heap %u RP %s\n",ESP.getFreeHeap(),CSTR(s));
	int i=0;
	int j=0;
	string rv;
//    Serial.printf("heap %u IPlen=%d\n",ESP.getFreeHeap(),s.size());
    rv.reserve((s.size()*115)/100);
//    Serial.printf("heap %u IPlen=%d capacity=%d\n",ESP.getFreeHeap(),s.size(),rv.capacity());
	while(i < s.size()){
        if(s[i]=='%'){
            if(j){
                string v=s.substr(j,i-j);
//                Serial.printf("pair @ %d %d %s\n",i,j,CSTR(v));
                if(_cb.count(v)) rv.append(_cb[v]);
                else rv.append("%"+v+"%");
                j=0;
            }
            else j=i+1;
        } else if(!j) rv.push_back(s[i]);
        i++;
	}
    rv.shrink_to_fit();
//    Serial.printf("heap %u OUT sz=%d\n%s\n",ESP.getFreeHeap(),rv.size(),CSTR(rv));
	return rv.c_str();
}

void H4P_UPNPServer::setBothNames(const string& hostname,const string& friendly){ 
    _pWiFi->_setPersistentValue(nameTag(),friendly,false);
    _pWiFi->host(hostname);
}