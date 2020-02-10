/*
 MIT License

Copyright (c) 2019 Phil Bowles <H48266@gmail.com>
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
#ifndef H4P_UPNPSwitch_HO
#define H4P_UPNPSwitch_HO

#include<H4PCommon.h>
#include<H4P_SerialCmd.h>
#include<H4P_BasicSwitch.h>
#include<H4P_WiFi.h>
#ifndef H4P_NO_WIFI

#include<H4P_AsyncWebServer.h>

class H4P_UPNPSwitch: public H4P_BasicSwitch {
        AsyncUDP 	    _udp;
        IPAddress		_ubIP;
        
        VSCMD(_friendly);

            string              _uuid="uuid:";
            string              _urn="Belkin:";

            vector<string>      _pups={
                "",
                "upnp:rootdevice"
            };

            string              _name;
            string              _soap;
            string              _ucom;
            string              _xml;

            string          __makeUSN(const string& s);
            string          __upnpCommon(const string& usn);
            void            __upnpSend(uint32_t mx,const string s,IPAddress ip,uint16_t port);

            void            broadcast(uint32_t mx,const string s){ __upnpSend(mx,s,_ubIP,1900); }

            void            _hookIn();
            void            _listenUDP();
            void            _notify(const string& s);
            void            _upnp(AsyncWebServerRequest *request);

            void            init();
            void            offline();
    public:
        H4P_UPNPSwitch(string name="",uint8_t pin=RELAY_BUILTIN,H4GM_SENSE sense=ACTIVE_HIGH, uint8_t initial=OFF,H4BS_FN_SWITCH f=[](bool){}):
            _name(name),
            H4P_BasicSwitch(pin,sense,initial,f){            
                _pups.push_back(_urn+"device:controllee:1");
                _pups.push_back(_urn+"service:basicevent:1");
                _pid=upnptag();
                _names = {
                    {H4P_TRID_SOAP, "SOAP"},
                    {H4P_TRID_NTFY, "NTFY"}
                };
            }
         
            void        friendlyName(const string& name);

};
    extern __attribute__((weak)) H4P_UPNPSwitch h4upnp;
#endif

#endif // H4P_UPNPSwitch_H