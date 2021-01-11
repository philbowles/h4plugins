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
#ifdef ARDUINO_ARCH_ESP8266
    #include<ESP8266WiFi.h>
    #include<ESP8266mDNS.h>
    #include<ESPAsyncTCP.h>
    #include<ESPAsyncUDP.h>
#else
    #include<WiFi.h>
    #include<AsyncTCP.h>
    #include<AsyncUDP.h>
    #include<ESPmDNS.h>
    #include<map> // WHY???
#endif
#include<DNSServer.h>
#include<ArduinoOTA.h>
#include<H4P_SerialCmd.h>

class H4P_WiFi: public H4Plugin{
                string     _device;
//
                VSCMD(_change);
                VSCMD(_host);
                VSCMD(_host2);

                string      HAL_WIFI_chipID();
                void        HAL_WIFI_setHost(const string& host);
//
                void        _coreStart();
                void        _gotIP();
                void        _lostIP();
        static  void        _wifiEvent(WiFiEvent_t event);
                void        _start() override;
                void        _stop() override;
                void        _hookIn() override;
    public:
                void        HAL_WIFI_startSTA(); // ESP32 FFS
//          included here against better wishes due to compiler bug https://gcc.gnu.org/bugzilla/show_bug.cgi?id=89605
#if H4P_USE_WIFI_AP
                void        _save(const string& s){ H4P_SerialCmd::write("/"+s,_cb[s]); }
                void        startAP();

        H4P_WiFi(H4_FN_VOID onC=nullptr,H4_FN_VOID onD=nullptr): H4Plugin(wifiTag(),onC,onD){
            _cb[ssidTag()]=uppercase(h4Tag());
            _cb[pskTag()]=uppercase(h4Tag());
#else
        H4P_WiFi(string ssid,string psk,string device="",H4_FN_VOID onC=nullptr,H4_FN_VOID onD=nullptr): _device(device),H4Plugin(wifiTag(),onC,onD){
            _cb[ssidTag()]=ssid;
            _cb[pskTag()]=psk;
#endif
            _factoryHook=[this](){ clear(); };
            _rebootHook=[this](){ _downHooks(); };
            _cmds={
                {_pName,    { H4PC_H4, _subCmd, nullptr}},
                {"clear",   { _subCmd, 0, CMD(clear)}},
                {"change",  { _subCmd, 0, CMDVS(_change)}},
                {"host",    { _subCmd, 0, CMDVS(_host)}}
            };
        }                
                void        clear();
                void        change(string ssid,string psk);
                void        host(const string& host){ _setPersistentValue(deviceTag(),host,true); }
                void        setBothNames(const string& host,const string& friendly);
                void        show() override { 
                    reply("Device %s Mode=%d Status: %d",CSTR(_cb[deviceTag()]),WiFi.getMode(),WiFi.status());
                    reply("SSID %s PSK=%s",CSTR(WiFi.SSID()),CSTR(WiFi.psk()));
                }
//          syscall only        
                bool        _getPersistentValue(string v,string prefix);
                void        _setPersistentValue(string n,string v,bool reboot);
};

extern __attribute__((weak)) H4P_WiFi h4wifi;