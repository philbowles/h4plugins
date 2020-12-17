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
#include<H4PCommon.h>
#include<H4P_SerialCmd.h>

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
            void        _greenLight() override {}
    public:
        string          _id;
        bool            _here=false;

        H4PDetector(const string& pid,const string& id,H4BS_FN_SWITCH f): _id(id),_f(f),H4Plugin(pid){}
                        
        bool        isPresent(){ return _here; }
        void show() override { reply("%s (%s) %s Present",CSTR(_pName),CSTR(_id),_here ? "":"NOT"); }
};

class H4P_UPNPDetector: public H4PDetector {
    protected:
                void        _hookIn() override;
                void        _start() override;
    public:
        H4P_UPNPDetector(const string& friendly,const string& usn,H4BS_FN_SWITCH f=nullptr): H4PDetector(friendly,usn,f){}
};

class H4P_UPNPDetectorSource: public H4P_UPNPDetector{
    public:
        H4P_UPNPDetectorSource(const string& pid,const string& id);
};

#ifdef ARDUINO_ARCH_ESP8266
class H4P_IPDetector: public H4PDetector {
        static  bool _inflight;

        static  struct ping_option pop;

        static  void        _ping_recv_cb(void *opt, void *resp);

    protected:
                void        _hookIn() override;
                void        _start() override;
    public:
        H4P_IPDetector(const string& friendly,const string& ip,H4BS_FN_SWITCH f=nullptr): H4PDetector(friendly,ip,f){}
};
class H4P_IPDetectorSource: public H4P_IPDetector{
    public:
        H4P_IPDetectorSource(const string& pid,const string& id);
};
#ifdef H4P_USE_OTA
class H4P_MDNSDetector: public H4PDetector {
        string _service;
        string _protocol;
        
        static unordered_map<string,H4P_MDNSDetector*> localList;
        
        static void MDNSServiceQueryCallback(MDNSResponder::MDNSServiceInfo serviceInfo, MDNSResponder::AnswerType answerType, bool p_bSetContent) {
            if(answerType ==  MDNSResponder::AnswerType::IP4Address){
                string who=replaceAll(serviceInfo.hostDomain(),".local","");
                if(localList.count(who)){ localList[who]->_inout(p_bSetContent); }
            }
        }   
    protected:
                void        _hookIn() override;
                void        _start() override;
    public:
        H4P_MDNSDetector(const string& friendly,const string& service,const string& protocol,H4BS_FN_SWITCH f=nullptr);
};

class H4P_H4Detector: public H4P_MDNSDetector {
    public:
        H4P_H4Detector(const string& local,H4BS_FN_SWITCH f=nullptr): H4P_MDNSDetector(local,"arduino","tcp",f){}
};

class H4P_H4DetectorSource: public H4P_H4Detector{
    public:
        H4P_H4DetectorSource(const string& local);
};

#endif // OTA
#endif // esp8266
#endif // H4P_PresenceDetector_HO