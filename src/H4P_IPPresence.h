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
#ifndef H4P_IPPresence_HO
#define H4P_IPPresence_HO

#include<H4PCommon.h>
#include<H4P_SerialCmd.h>
#include<H4P_WiFiSelect.h>
#include<H4P_WiFi.h>
#ifndef H4P_NO_WIFI

#define H4P_IPPD_RATE   5000

class H4P_IPPresence: public H4PluginService{
        H4BS_FN_SWITCH  _detect;
        string          _ip;
        uint32_t        _present=0;
        uint32_t        _ping=0;
        H4_TIMER        _pinger;
        void _hookIn() override;
        void _ping_recv_cb(void *opt, void *resp);
    public:
        H4P_IPPresence(const string& ip,const string& friendly,H4BS_FN_SWITCH onDetect):_ip(ip), _detect(onDetect), H4PluginService([](){},[](){}){
            _pid=friendly;
            _names={{H4P_TRID_IPPD,"PING"}};
        }
                void        start() override;
                void        stop() override;
                bool        isPresent(){ return _present; }
};

    extern __attribute__((weak)) H4P_IPPresence h4ippd;
#endif

#endif // H4P_IPPresence_H