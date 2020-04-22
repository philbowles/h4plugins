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

/*
0000   01 00 5e 7f ff fa a0 20 a6 17 d8 48 08 00 45 00   ..^.... ...H..E.
0010   01 a1 00 69 00 00 01 11 05 d9 c0 a8 01 68 ef ff   ...i.........h..
0020   ff fa 07 6c 07 6c 01 8d 7e d3 4e 4f 54 49 46 59   ...l.l..~.NOTIFY
0030   20 2a 20 48 54 54 50 2f 31 2e 31 0d 0a 48 4f 53    * HTTP/1.1..HOS
0040   54 3a 32 33 39 2e 32 35 35 2e 32 35 35 2e 32 35   T:239.255.255.25
0050   30 3a 31 39 30 30 0d 0a 4e 54 53 3a 73 73 64 70   0:1900..NTS:ssdp
0060   3a 61 6c 69 76 65 0d 0a 4e 54 3a 75 70 6e 70 3a   :alive..NT:upnp:
0070   72 6f 6f 74 64 65 76 69 63 65 0d 0a 43 41 43 48   rootdevice..CACH
0080   45 2d 43 4f 4e 54 52 4f 4c 3a 20 6d 61 78 2d 61   E-CONTROL: max-a
0090   67 65 3d 33 30 30 0d 0a 45 58 54 3a 0d 0a 4c 4f   ge=300..EXT:..LO
00a0   43 41 54 49 4f 4e 3a 20 68 74 74 70 3a 2f 2f 31   CATION: http://1
00b0   39 32 2e 31 36 38 2e 31 2e 31 30 34 3a 38 30 2f   92.168.1.104:80/
00c0   77 65 0d 0a 4f 50 54 3a 20 22 68 74 74 70 3a 2f   we..OPT: "http:/
00d0   2f 73 63 68 65 6d 61 73 2e 75 70 6e 70 2e 6f 72   /schemas.upnp.or
00e0   67 2f 75 70 6e 70 2f 31 2f 30 2f 22 3b 20 6e 73   g/upnp/1/0/"; ns
00f0   3d 30 31 0d 0a 30 31 2d 4e 4c 53 3a 20 31 37 44   =01..01-NLS: 17D
0100   38 34 38 0d 0a 53 45 52 56 45 52 3a 20 46 6f 72   848..SERVER: For
0110   6b 65 64 20 41 73 79 6e 63 57 65 62 53 65 72 76   ked AsyncWebServ
0120   65 72 2c 20 55 50 6e 50 2f 31 2e 30 2c 20 48 34   er, UPnP/1.0, H4
0130   2f 30 2e 35 2e 34 0d 0a 58 2d 48 34 2d 44 65 76   /0.5.4..X-H4-Dev
0140   69 63 65 3a 20 48 34 2d 31 37 44 38 34 38 0d 0a   ice: H4-17D848..
0150   58 2d 48 34 2d 43 68 69 70 3a 20 31 37 44 38 34   X-H4-Chip: 17D84
0160   38 0d 0a 58 2d 55 73 65 72 2d 41 67 65 6e 74 3a   8..X-User-Agent:
0170   20 72 65 64 73 6f 6e 69 63 0d 0a 55 53 4e 3a 20    redsonic..USN: 
0180   75 75 69 64 3a 53 6f 63 6b 65 74 2d 31 5f 30 2d   uuid:Socket-1_0-
0190   75 70 6e 70 31 37 44 38 34 38 3a 3a 75 70 6e 70   upnp17D848::upnp
01a0   3a 72 6f 6f 74 64 65 76 69 63 65 0d 0a 0d 0a      :rootdevice....
*/
#include<H4P_UPNPServer.h>
#ifndef H4P_NO_WIFI

void H4P_UPNPServer::_hookIn(){
    DEPEND(asws);
    REQUIREBT;
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
        _cb[nameTag()]=_name;
        h4wifi._getPersistentValue(nameTag(),"upnp ");
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
        _notify("alive"); // TAG
        h4.every(H4P_UDP_REFRESH / 3,[this](){ _notify("alive"); },nullptr,H4P_TRID_NTFY,true); // TAG
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
        _cb[stateTag()]=stringFromInt(_btp->state());
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
//  diag sense check
//     vector<string> dcs=split(nfy,"\r\n");
//     Serial.printf("N lines=%d NCR = %d, NCR=%d\n",dcs.size(),count(nfy.begin(), nfy.end(), '\r'),count(nfy.begin(), nfy.end(), '\n'));
//
          broadcast(H4P_UDP_JITTER,CSTR(nfy));
    });
}

string H4P_UPNPServer::replaceParams(const string& s){ // oh for a working regex...
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

#endif