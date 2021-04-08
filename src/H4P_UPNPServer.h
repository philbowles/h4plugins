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
#pragma once

#include<H4Service.h>
#include<H4P_WiFi.h>

STAG(alive);

class H4P_UPNPServer: public H4Service {
            H4P_WiFi*           _pWiFi;
            AsyncUDP 	        _udp;
            IPAddress		    _ubIP=IPAddress(239,255,255,250);;

//                VSCMD(_friendly);

                string          _uuid="uuid:";
                string          _urn="Belkin:";
                string          _udn;

                vector<string>  _pups={"",rootTag()};

                string          _soap;
                string          _ucom;
                string          _xml;

                string          __makeUSN(const string& s);
                string          __upnpCommon(const string& usn);
                void            __upnpSend(uint32_t mx,const string s,IPAddress ip,uint16_t port);

        virtual void            _handleEvent(const string& svc,H4PE_TYPE t,const string& msg) override;
                void            _handlePacket(string p,IPAddress ip,uint16_t port);
                void            _listenUDP();
                void            _notify(const string& s);
                void            _upnp(AsyncWebServerRequest *request);

        static  string 	        replaceParamsFile(const string &f){ return h4preplaceparams(CSTR(H4P_SerialCmd::read(f))); }
    public:                
        H4P_UPNPServer(const string& name=""): H4Service(upnpTag(),H4PE_GVCHANGE|H4PE_VIEWERS){
            h4p.gvSetstring(nameTag(),name,true);
            _pWiFi=depend<H4P_WiFi>(wifiTag());
        }
                void            friendlyName(const string& name){ h4p[nameTag()]=name; }
#if H4P_LOG_MESSAGES
                void           info() override;
#endif
                void           svcUp() override;
                void           svcDown() override;
//          _syscall only
                void           _broadcast(uint32_t mx,const string s){ __upnpSend(mx,s,_ubIP,1900); }
                void           _listenTag(const string& tag,const string& value);
                void           _init() override;
};