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
copies of the Software, and to permit persons to whom the Software iss
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
#ifndef H4P_PresenceDetector_HO
#define H4P_PresenceDetector_HO

#include<H4P_WiFiSelect.h>
#ifndef H4P_NO_WIFI

#include<H4PCommon.h>
#include<H4P_WiFi.h>
#include<H4P_SerialCmd.h>

#include<H4P_UPNPServer.h>

#define H4P_IPPD_RATE   5000

using H4P_FN_PRESENCE   = function<void(const string,bool)>;

enum H4P_PD_TYPE {
    H4P_DETECT_IP,
    H4P_DETECT_USN
};

struct H4P_PRESENCE {
    H4P_PD_TYPE     t;
    string          n;
    H4BS_FN_SWITCH  f;
};

using H4P_PRESENCE_MAP = unordered_map<string,H4P_PRESENCE>;

class H4PDetector: public H4Plugin{
    protected:
        H4_TIMER        _pinger;
        H4BS_FN_SWITCH  _f;

            void            _inout(bool b){
                if(b!=_here){
                    if(_f) _f(b);
                    _here=b;
                    SYSEVENT(b ? H4P_LOG_PD_ENTER:H4P_LOG_PD_LEAVE ,"pd",_pName,"%s",b ? "IN":"OUT");
                }
            }
            void        _stop() override{
                h4.cancel(_pinger);
                _downHooks();
            };
    public:
        string          _id;
        bool            _here=false;
        H4PDetector(const string& pid,const string& id,H4BS_FN_SWITCH f): _id(id),_f(f),H4Plugin(pid){}
};

class H4P_IPDetector: public H4PDetector {
        static  bool _inflight;

        static  struct ping_option pop;

        static  void        _ping_recv_cb(void *opt, void *resp);

    protected:
                void        _hookIn() override;
                void        _start() override;
                void        _greenLight() override {}
    public:
        H4P_IPDetector(const string& friendly,const string& ip,H4BS_FN_SWITCH f): H4PDetector(friendly,ip,f){}
};

class H4P_UPNPDetector: public H4PDetector {
    protected:
                void        _hookIn() override;
                void        _start() override;
                void        _greenLight() override {}
    public:
        H4P_UPNPDetector(const string& friendly,const string& usn,H4BS_FN_SWITCH f): H4PDetector(friendly,usn,f){}
};

using H4P_PD_MAP       =unordered_map<string,H4PDetector*>;

class H4P_PresenceDetector: public H4Plugin{
        H4P_PD_MAP          _register;
    public:
        H4P_PresenceDetector(H4P_PRESENCE_MAP pm): H4Plugin("ispy"){
            for(auto const& p:pm){
                H4PDetector* d;
                switch(p.second.t){
                    case H4P_DETECT_IP:
                        d=new H4P_IPDetector(p.first,p.second.n,p.second.f);
                        break;
                    case H4P_DETECT_USN:
                        d=new H4P_UPNPDetector(p.first,p.second.n,p.second.f);
                        break;
                }
                _register[p.first]=d;
            }
            pm.clear();
        }
                bool        isPresent(const string& friendly){ 
                    if(_register.count(friendly)) return _register[friendly]->_here;
                    return false;
                }
                void show() override {
                    reply("Present:");
                    for(auto const& r:_register) if(r.second->_here) reply("%s (%s)",CSTR(r.first),CSTR(r.second->_id));
                }
};
    extern __attribute__((weak)) H4P_PresenceDetector h4pd;
#endif

#endif // H4P_PresenceDetector_H