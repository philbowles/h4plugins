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

struct h4device {
  string    ip;
  string    chip;
  H4_TIMER  pinger;
  uint32_t  mx;
};

using H4P_GANG_MEMBERS = std::unordered_map<string,h4device>;

class H4P_Gangmaster: public H4Plugin {
        H4P_UPNPServer*     _pUPNP;
        H4P_GANG_MEMBERS    _gang;

            void   _handleEvent(H4PID pid,H4P_EVENT_TYPE type,const string &msg) override { 
                if(type==H4P_EVENT_H4_ENTER){
                    vector<string> vs=split(msg,",");
                    uint32_t mx=H4PAYLOAD_INT;
                    string gm=trim(vs[0]);
                    if(_gang.count(gm)) h4.cancel(_gang[gm].pinger);
                    struct h4device newkid={
                            ltrim(split(split(vs[2],"/")[2],":")[0]),
                            ltrim(vs[1]),
                            h4.once(mx,[=](){ PEVENT(H4P_EVENT_H4_LEAVE,"%s",CSTR(gm)); }),
                            mx};
                    _gang[gm]=newkid;
                } else _gang.erase(msg); 
            }

            void _greenLight() override {}
            
            void _hookIn() override {
                _pUPNP=h4pdepend<H4P_UPNPServer>(this,H4PID_UPNP);
                _pUPNP->_listenTag("X-H4-DEVICE","*",[this](uint32_t && mx,H4P_CONFIG_BLOCK && uh,bool && direction){
                    if(uh["USN"].find(rootTag())!=string::npos) {
                        if(direction) PEVENT(H4P_EVENT_H4_ENTER,"%s,%s,%s,%d",CSTR(uh["X-H4-DEVICE"]),CSTR(uh["X-H4-CHIP"]),CSTR(uh["LOCATION"]),mx); // stag it
                        else PEVENT(H4P_EVENT_H4_LEAVE,"%s",CSTR(uh["X-H4-DEVICE"])); // stag it
                    }
                });
            }

            void _start() override {
                _pUPNP->broadcast(50,h4cmd.read("/srch.txt"));
                _upHooks();
            }
    public:
        H4P_Gangmaster(): H4Plugin(H4PID_GANG,H4P_EVENT_H4_ENTER | H4P_EVENT_H4_LEAVE){}

            void show()override { for(auto const& g:_gang){ reply("%s chip:%s ip:%s mx=%d\n",CSTR(g.first),CSTR(g.second.chip),CSTR(g.second.ip),g.second.mx); } }
};