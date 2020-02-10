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
#ifndef H4P_WiFi_H
#define H4P_WiFi_H

extern void h4FactoryReset();

class H4P_WiFi: public H4PluginService{
                DNSServer* _dnsServer;
//
                VSCMD(_change);
                VSCMD(_dump);
                VSCMD(_host);
//
                string      _getChipID(){ return stringFromInt(ESP.getChipId(),"%06X"); }
                void        _gotIP();
                void        _greenLight() override;
                void        _hookIn() override;
                void        _lostIP();
                void        _scan();
                void        _startSTA();
                void        _startAP();
        static  void        _wifiEvent(WiFiEvent_t event);

    public:
//          included here aginst better wishes due to compiler bug https://gcc.gnu.org/bugzilla/show_bug.cgi?id=89605
        H4P_WiFi(string ssid,string psk,string device="",H4_FN_VOID onC=[](){},H4_FN_VOID onD=[](){}): H4PluginService(onC,onD){
            _cb[ssidtag()]=ssid;
            _cb["psk"]=psk;
            _cb[devicetag()]=device;

            _pid=wifitag();
            subid=H4PC_WIFI;
            _names={ 
                    {H4P_TRID_WIFI,uppercase(_pid)},
                    {H4P_TRID_WFAP,"WFAP"},
                    {H4P_TRID_HOTA,"HOTA"}
            };

            _local={
                {"clear",   { H4PC_WIFI, 0, CMD(clear)}},
                {"dump",    { H4PC_WIFI, 0, CMDVS(_dump)}},
                {"factory", { H4PC_ROOT, 0, CMD(h4FactoryReset) }},
                {"change",  { H4PC_WIFI, 0, CMDVS(_change)}},             
                {"host",    { H4PC_WIFI, 0, CMDVS(_host)}},             
                {_pid,      { H4PC_SHOW, 0, CMD(show) }},
                {"spif",    { H4PC_SHOW, 0, CMD(showSPIFFS)}}
            };
        }                

                void     clear();
                void     change(string ssid,string psk);
                void     getPersistentValue(string v,string prefix);
                void     host(string h){ setPersistentValue(devicetag(),h,true); }
                void     setPersistentValue(string n,string v,bool reboot);
                void     start() override;
                void     stop() override;
                void     show() { WiFi.printDiag(Serial); }
//          SPIFFS
        static  string   read(const string& fn);
        static  string   replaceParams(const string& s);
        static  string 	 replaceParamsFile(const string &f){ return replaceParams(CSTR(read(f))); }
        static  void     write(const string& fn,const string& data);
                void     showSPIFFS();
};
    extern __attribute__((weak)) H4P_WiFi h4wifi;

#endif // H4P_WiFi_H