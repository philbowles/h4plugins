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

#include<H4PCommon.h>
#include<H4P_WiFi.h>
#include<H4P_BinaryThing.h>

STAG(alive);

using H4P_FN_TAGMATCH   =function<void(uint32_t mx,H4P_CONFIG_BLOCK,bool)>;
using H4P_TAG_MAP       =unordered_map<string,pair<string,H4P_FN_TAGMATCH>>;

class H4P_UPNPServer: public H4Plugin {
        H4P_BinaryThing* _btp;
        H4P_WiFi*       _pWiFi;
        AsyncUDP 	    _udp;
        IPAddress		_ubIP;
        H4P_TAG_MAP     _detect;

//        unordered_set<string>     _otherH4s;

                VSCMD(_friendly);
                VSCMD(_host2);

                string          _uuid="uuid:";
                string          _urn="Belkin:";

            vector<string>      _pups={
                    "",
                    "upnp:rootdevice"
                };

                string          _name;
                string          _soap;
                string          _ucom;
                string          _xml;

                string          __makeUSN(const string& s);
                string          __upnpCommon(const string& usn);
                void            __upnpSend(uint32_t mx,const string s,IPAddress ip,uint16_t port);

                void            _handleEvent(H4PID pid,H4P_EVENT_TYPE type,const string &msg) override;
                void            _handlePacket(string p,IPAddress ip,uint16_t port);
                void            _listenUDP();
                void            _notify(const string& s);
                void            _upnp(AsyncWebServerRequest *request);

                void            _start() override;
                void            _stop() override;
                void            _greenLight() override {}; // dont autostart!

        static  string 	        replaceParamsFile(const string &f){ return h4preplaceparams(CSTR(H4P_SerialCmd::read(f))); }
    public:                
        H4P_UPNPServer(const string& name="",H4_FN_VOID onC=nullptr,H4_FN_VOID onD=nullptr): _name(name), H4Plugin(H4PID_UPNP,H4P_EVENT_FACTORY,onC,onD){
            _pups.push_back(_urn+"device:controllee:1");
            _pups.push_back(_urn+"service:basicevent:1");
            _ubIP=IPAddress(239,255,255,250);
            _addLocals({ 
                {_pName,    { H4PC_H4, _pid, nullptr}},
                {"name",    {_pid, 0, CMDVS(_friendly)}},
                {"host2",   {_pid, 0, CMDVS(_host2)}}
            });
        }

                void           broadcast(uint32_t mx,const string s){ __upnpSend(mx,s,_ubIP,1900); }
                void           friendlyName(const string& name);
                void           setBothNames(const string& host,const string& friendly);
                void           show() override { 
                    reply("Name: %s",CSTR(_cb[nameTag()]));
    //                 reply("Neighbours:");
    //                 for(auto const& o:_otherH4s) reply(CSTR(o));
                }
//          _syscall only
             void           _listenTag(const string& tag,const string& value,H4P_FN_TAGMATCH f){
                 PLOG("listen TAG %s=%s",CSTR(tag),CSTR(value));
                 _detect[tag]=make_pair(value,f);
            }
             void            _hookIn() override;
};

//extern __attribute__((weak)) H4P_UPNPServer h4upnp;