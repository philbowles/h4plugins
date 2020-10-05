/*
 MIT License

Copyright (c) 2020 Phil Bowles <h4plugins@gmail.com>
   github     https://github.com/philbowles/esparto
   blog       https://8266iot.blogspot.com     
   groups     https://www.facebook.com/groups/esp8266questions/
              https://www.facebook.com/Esparto-Esp8266-Firmware-Support-2338535503093896/
                			  

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
#include<H4PCommon.h>
#include<H4P_WiFi.h>
#include <H4P_SerialCmd.h>
#include<H4P_AsyncWebserver.h>

#ifndef H4P_NO_WIFI

uint32_t H4P_WiFi::_change(vector<string> vs){ return guardString2(vs,[this](string a,string b){ change(a,b); return H4_CMD_OK; }); }

bool H4P_WiFi::_getPersistentValue(string v,string prefix){
    string persistent=replaceAll(H4P_SerialCmd::read("/"+v),"\r\n","");
    _cb[v]=persistent.size() ? persistent:string(prefix)+_cb[chipTag()];
    return persistent.size();
}

void H4P_WiFi::_gotIP(){
    _discoDone=false;
    _cb["ip"]=WiFi.localIP().toString().c_str();
    _cb[ssidTag()]=CSTR(WiFi.SSID());
    _cb[pskTag()]=CSTR(WiFi.psk());

    string host=_cb[deviceTag()];
#ifdef H4P_USE_OTA
    h4.every(H4WF_OTA_RATE,[](){ ArduinoOTA.handle(); },nullptr,H4P_TRID_HOTA,true);
    _setHost(host);
    if(MDNS.begin(CSTR(host))) {
        MDNS.addService("h4","tcp",666);
        MDNS.addServiceTxt("h4","tcp","id",CSTR(_cb[chipTag()]));
        MDNS.addServiceTxt("h4","tcp","ip",CSTR(_cb["ip"]));
    } else Serial.println("Error starting mDNS");
  	ArduinoOTA.setHostname(CSTR(host));
	ArduinoOTA.setRebootOnSuccess(false);	
	ArduinoOTA.begin();
#endif
    _cb.erase("opts"); // lose any old AP ssids
    Serial.printf("IP=%s\n",CSTR(_cb["ip"])); // unconditional
    H4EVENT("IP=%s",CSTR(_cb["ip"]));
    _upHooks();
}

void H4P_WiFi::_greenLight() { 
    if(isLoaded(upnpTag())) h4cmd.addCmd("host2",_subCmd,0,CMDVS(_host2));
    start();
}

void H4P_WiFi::_hookIn(){
    _cb[chipTag()]=_getChipID();
    _cb[boardTag()]=replaceAll(H4_BOARD,"ESP8266_","");
    if(!_getPersistentValue(deviceTag(),"H4-")) if(_device!="") _cb[deviceTag()]=_device;
    H4EVENT("Device %s",CSTR(_cb[deviceTag()]));
    _getPersistentValue("h4sv","*");
    WiFi.onEvent(_wifiEvent);
}

uint32_t H4P_WiFi::_host(vector<string> vs){
    return guard1(vs,[this](vector<string> vs){
        return ([this](string h){
            host(h); 
            return H4_CMD_OK;
        })(H4PAYLOAD);
    });
}

uint32_t H4P_WiFi::_host2(vector<string> vs){ return guardString2(vs,[this](string a,string b){ h4asws._setBothNames(a,b); return H4_CMD_OK; }); }

void H4P_WiFi::_lostIP(){
    if(!_discoDone){
        _downHooks();
        _discoDone=true;
    }
}

void H4P_WiFi::_scan(){ // check 4 common hoist
    WiFi.enableSTA(true);
    int n=WiFi.scanNetworks();

    for (uint8_t i = 0; i < n; i++){
        char buf[128];
        snprintf(buf,127,"<option>%s</option>",CSTR(WiFi.SSID(i)));
        _cb["opts"].append(buf); // delete later!!!
    }
    WiFi.scanDelete();
    WiFi.enableSTA(false); // force AP only
}

void H4P_WiFi::_setPersistentValue(string n,string v,bool reboot){
    string oldvalue=_cb[n];
    if(oldvalue!=v){
        H4P_SerialCmd::write("/"+n,v);
        if(reboot) h4reboot();
    }
}

void H4P_WiFi::_start(){
    H4Plugin::_factoryChain.push_back(_clearAP);
    if(SPIFFS.exists("/ap")){
        stop();
        _startAP();
    } else _mcuStart();
}

void H4P_WiFi::_startAP(){
    _dnsServer= new DNSServer;
    WiFi.mode(WIFI_AP);
    WiFi.enableSTA(false); // force AP only
    WiFi.softAP(CSTR(_cb[deviceTag()]));
    delay(0); // solve 192.168.244.1 probelm ?	
    _dnsServer=new DNSServer;
    _dnsServer->start(53, "*", WiFi.softAPIP());
    h4.every(1000,[this](){ _dnsServer->processNextRequest(); },nullptr,H4P_TRID_WFAP,true);
    _cb["ip"]=WiFi.softAPIP().toString().c_str();
    _scan();
    _upHooks();
}

void H4P_WiFi::_stopCore(){
    h4.cancelSingleton({H4P_TRID_HOTA,H4P_TRID_WFAP});
    if(WiFi.getMode() & WIFI_AP) {
        if(_dnsServer){
            _dnsServer->stop();
            delete _dnsServer;
            _dnsServer=nullptr;
            _downHooks();
        }
    }
}

#ifdef ARDUINO_ARCH_ESP8266
//
//      ESP8266
//
void H4P_WiFi::_setHost(const string& host){ WiFi.hostname(CSTR(host)); }

string H4P_WiFi::_getChipID(){ return stringFromInt(ESP.getChipId(),"%06X"); }

void H4P_WiFi::clear(){ 
	stop();
    WiFi.disconnect(true); 
	ESP.eraseConfig();
    SPIFFS.remove(CSTR(string("/"+string(deviceTag()))));
    _clearAP();
}

void H4P_WiFi::_startSTA(){
    _clearAP();
    WiFi.mode(WIFI_STA);
	WiFi.setSleepMode(WIFI_NONE_SLEEP);
    WiFi.enableAP(false); 
	WiFi.setAutoConnect(true);
	WiFi.setAutoReconnect(true);
    WiFi.begin(CSTR(_cb[ssidTag()]),CSTR(_cb[pskTag()]));
}

void H4P_WiFi::_mcuStart(){ if(WiFi.getMode()==WIFI_OFF || WiFi.SSID()=="") _startSTA(); }

void H4P_WiFi::_stop(){
    _stopCore();
	WiFi.mode(WIFI_OFF);
}
/*
ESP8266

    WIFI_EVENT_STAMODE_CONNECTED = 0,
    WIFI_EVENT_STAMODE_DISCONNECTED,
    WIFI_EVENT_STAMODE_AUTHMODE_CHANGE,
    WIFI_EVENT_STAMODE_GOT_IP,
    WIFI_EVENT_STAMODE_DHCP_TIMEOUT,
    WIFI_EVENT_SOFTAPMODE_STACONNECTED,
    WIFI_EVENT_SOFTAPMODE_STADISCONNECTED,
    WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED,
    WIFI_EVENT_MAX,
    WIFI_EVENT_ANY = WIFI_EVENT_MAX,
    WIFI_EVENT_MODE_CHANGE

*/

void H4P_WiFi::_wifiEvent(WiFiEvent_t event) {
    switch(event) {
        case WIFI_EVENT_STAMODE_DISCONNECTED:
			h4.queueFunction([](){ h4wifi._lostIP(); });
            break;    
		case WIFI_EVENT_STAMODE_GOT_IP:
			h4.queueFunction([](){ h4wifi._gotIP(); });
			break;
	}
}
#else
//
//      ESP32
//
void H4P_WiFi::_setHost(const string& host){ WiFi.setHostname(CSTR(host)); }

string H4P_WiFi::_getChipID(){
    uint64_t macAddress = ESP.getEfuseMac();
    uint64_t macAddressTrunc = macAddress << 40;
    return stringFromInt(macAddressTrunc >> 40,"%06X");
}

void H4P_WiFi::clear(){
    _stop();
	WiFi.disconnect(true,true);
    _clearAP();
}
/*
void H4P_WiFi::_scan(){ // coalescee
    int n=WiFi.scanNetworks();

    for (uint8_t i = 0; i < n; i++){
        char buf[128];
        snprintf(buf,127,"<option>%s</option>",CSTR(WiFi.SSID(i)));
        _cb["opts"].append(buf); // delete later!!!
    }
    WiFi.scanDelete();
    WiFi.enableSTA(false); // force AP only
}
*/
void H4P_WiFi::_startSTA(){
    _clearAP();
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.enableAP(false); 
	WiFi.setAutoReconnect(true);
    WiFi.begin(CSTR(_cb[ssidTag()]),CSTR(_cb[pskTag()]));
}

void H4P_WiFi::_mcuStart(){ if(WiFi.getMode()==WIFI_OFF || WiFi.SSID()=="") _startSTA(); }

void H4P_WiFi::_stop(){
    _stopCore();
    WiFi.disconnect(true,false);
}
/* ESP32
* WiFi Events
0  SYSTEM_EVENT_WIFI_READY               < ESP32 WiFi ready
1  SYSTEM_EVENT_SCAN_DONE                < ESP32 finish scanning AP
2  SYSTEM_EVENT_STA_START                < ESP32 station start
3  SYSTEM_EVENT_STA_STOP                 < ESP32 station stop
4  SYSTEM_EVENT_STA_CONNECTED            < ESP32 station connected to AP
5  SYSTEM_EVENT_STA_DISCONNECTED         < ESP32 station disconnected from AP
6  SYSTEM_EVENT_STA_AUTHMODE_CHANGE      < the auth mode of AP connected by ESP32 station changed
7  SYSTEM_EVENT_STA_GOT_IP               < ESP32 station got IP from connected AP
8  SYSTEM_EVENT_STA_LOST_IP              < ESP32 station lost IP and the IP is reset to 0
9  SYSTEM_EVENT_STA_WPS_ER_SUCCESS       < ESP32 station wps succeeds in enrollee mode
10 SYSTEM_EVENT_STA_WPS_ER_FAILED        < ESP32 station wps fails in enrollee mode
11 SYSTEM_EVENT_STA_WPS_ER_TIMEOUT       < ESP32 station wps timeout in enrollee mode
12 SYSTEM_EVENT_STA_WPS_ER_PIN           < ESP32 station wps pin code in enrollee mode
13 SYSTEM_EVENT_AP_START                 < ESP32 soft-AP start
14 SYSTEM_EVENT_AP_STOP                  < ESP32 soft-AP stop
15 SYSTEM_EVENT_AP_STACONNECTED          < a station connected to ESP32 soft-AP
16 SYSTEM_EVENT_AP_STADISCONNECTED       < a station disconnected from ESP32 soft-AP
17 SYSTEM_EVENT_AP_STAIPASSIGNED         < ESP32 soft-AP assign an IP to a connected station
18 SYSTEM_EVENT_AP_PROBEREQRECVED        < Receive probe request packet in soft-AP interface
19 SYSTEM_EVENT_GOT_IP6                  < ESP32 station or ap or ethernet interface v6IP addr is preferred
20 SYSTEM_EVENT_ETH_START                < ESP32 ethernet start
21 SYSTEM_EVENT_ETH_STOP                 < ESP32 ethernet stop
22 SYSTEM_EVENT_ETH_CONNECTED            < ESP32 ethernet phy link up
23 SYSTEM_EVENT_ETH_DISCONNECTED         < ESP32 ethernet phy link down
24 SYSTEM_EVENT_ETH_GOT_IP               < ESP32 ethernet got IP from connected AP
25 SYSTEM_EVENT_MAX

    WL_NO_SHIELD        = 255,   // for compatibility with WiFi Shield library
    WL_IDLE_STATUS      = 0,
    WL_NO_SSID_AVAIL    = 1,
    WL_SCAN_COMPLETED   = 2,
    WL_CONNECTED        = 3,
    WL_CONNECT_FAILED   = 4,
    WL_CONNECTION_LOST  = 5,
    WL_DISCONNECTED     = 6

*/

void H4P_WiFi::_wifiEvent(WiFiEvent_t event) {
    switch(event) {
        case SYSTEM_EVENT_STA_STOP:
        case SYSTEM_EVENT_STA_LOST_IP:
        case SYSTEM_EVENT_STA_DISCONNECTED:
			if(!(WiFi.getMode() & WIFI_AP)) h4.queueFunction([](){ h4wifi._lostIP(); }); // ? if?
			//h4.queueFunction([](){ h4wifi._lostIP(); }); // ? if?
            break;
		case SYSTEM_EVENT_STA_GOT_IP:
			h4.queueFunction([](){ h4wifi._gotIP(); });
			break;
	}
}
#endif

void H4P_WiFi::change(string ssid,string psk){ // add device / name?
    stop();
    _cb[ssidTag()]=ssid;
    _cb[pskTag()]=psk;
    _startSTA();
}

void H4P_WiFi::forceAP(){ 
    H4EVENT("AP MODE FORCED");
    h4cmd.write("/ap","");
    h4reboot();
} // tagify?

void H4P_WiFi::setBothNames(const string& host,const string& friendly){ h4asws._setBothNames(host,friendly); }

#endif