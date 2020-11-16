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
#ifndef H4P_WiFi_H
#define H4P_WiFi_H

#include<H4PCommon.h>

#include<H4P_WiFiSelect.h>
#ifndef ARDUINO_ARCH_STM32

#include <H4P_SerialCmd.h>

class H4P_WiFi: public H4Plugin{
                DNSServer* _dnsServer;
                string     _device;
//
                VSCMD(_change);
                VSCMD(_host);
                VSCMD(_host2);
//
        static  void        _clearAP(){ HAL_FS.remove("/ap"); }
                string      _getChipID();
                void        _gotIP();
                void        _lostIP();
                void        _mcuStart();
                void        _scan();
                void        _setHost(const string& host);
                void        _startSTA();
                void        _startAP();
                void        _stopCore();
        static  void        _wifiEvent(WiFiEvent_t event);

                void        _start() override;
                bool        _state() override { return WiFi.status() == WL_CONNECTED; }
                void        _stop() override;
                void        _greenLight() override;
                void        _hookIn() override;
    public:
//          included here against better wishes due to compiler bug https://gcc.gnu.org/bugzilla/show_bug.cgi?id=89605
        H4P_WiFi(string ssid,string psk,string device="",H4_FN_VOID onC=nullptr,H4_FN_VOID onD=nullptr): _device(device),H4Plugin(wifiTag(),onC,onD){
            _cb[ssidTag()]=ssid;
            _cb[pskTag()]=psk;
            _factoryHook=[this](){ clear(); };
            _cmds={
                {_pName,    { H4PC_H4, _subCmd, nullptr}},
                {"apmode",  { _subCmd, 0, CMD(forceAP)}},
                {"clear",   { _subCmd, 0, CMD(clear)}},
                {"change",  { _subCmd, 0, CMDVS(_change)}},
                {"host",    { _subCmd, 0, CMDVS(_host)}}
            };
        }                
                void     clear();
                void     change(string ssid,string psk);
                void     forceAP();
                void     host(const string& host){ _setPersistentValue(deviceTag(),host,true); }
                void     setBothNames(const string& host,const string& friendly);
                void     show() override { 
                    WiFi.printDiag(Serial);
                    reply("Device %s Status: %d",CSTR(_cb[deviceTag()]),WiFi.status());
                }
//          syscall only        
                bool     _getPersistentValue(string v,string prefix);
                void     _setPersistentValue(string n,string v,bool reboot);
};
    extern __attribute__((weak)) H4P_WiFi h4wifi;

#else
    #define H4P_NO_WIFI
#endif // stm32

#endif // H4P_WiFi_H
