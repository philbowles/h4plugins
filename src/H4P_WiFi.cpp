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
#include<H4P_SerialCmd.h>
#include<H4P_AsyncWebserver.h>

STAG(tcp)

/*
                                             H / W - D E P E N D E N T   F U N C T I O N S
*/
#ifdef ARDUINO_ARCH_ESP8266
//
//                                                                                                  ESP8266
//
string H4P_WiFi::HAL_WIFI_chipID(){ return stringFromInt(ESP.getChipId(),"%06X"); }

void H4P_WiFi::HAL_WIFI_setHost(const string& host){ WiFi.hostname(CSTR(host)); } // change ????

void H4P_WiFi::HAL_WIFI_startSTA(){
    WiFi.mode(WIFI_STA);
    WiFi.enableAP(false); 
	WiFi.setSleepMode(WIFI_NONE_SLEEP);
	WiFi.setAutoConnect(true);
	WiFi.setAutoReconnect(true);
    Serial.printf("HAL_WIFI_startSTA %s %s\n",CSTR(_cb[ssidTag()]),CSTR(_cb[pskTag()]));
    WiFi.begin(CSTR(_cb[ssidTag()]),CSTR(_cb[pskTag()]));
}

void H4P_WiFi::_start(){ Serial.printf("H4P_WiFi::_start() 8266\n"); _coreStart(); }
/*
#if H4P_USE_WIFI_AP
void H4P_WiFi::_coreStart(){ 
    auto mode=WiFi.getMode();
    Serial.printf("H4P_WiFi AP::::_coreStart() 8266 mode=%d psk=%s\n",mode,CSTR(WiFi.psk()));
    if(mode==WIFI_AP || WiFi.psk()=="H4") startAP();
}
#else
void H4P_WiFi::_coreStart(){ 
    auto mode=WiFi.getMode();
    Serial.printf("H4P_WiFi::_coreStart() 8266 mode=%d\n",mode);
    switch(mode){
        case WIFI_OFF:
            HAL_WIFI_startSTA();
            break;
        case WIFI_STA:
            if(WiFi.psk()=="H4") HAL_WIFI_startSTA();
            break;
        case WIFI_AP:
            HAL_WIFI_startSTA();
            break;
    }
}
#endif
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
//                                                                                                                  ESP32
//
string H4P_WiFi::HAL_WIFI_chipID(){
    uint64_t macAddress = ESP.getEfuseMac();
    uint64_t macAddressTrunc = macAddress << 40;
    return stringFromInt(macAddressTrunc >> 40,"%06X");
}

void H4P_WiFi::HAL_WIFI_setHost(const string& host){ WiFi.setHostname(CSTR(host)); }

void H4P_WiFi::HAL_WIFI_startSTA(){
    Serial.printf("H4P_WiFi::HAL_WIFI_startSTA 32 %s %s\n",CSTR(_cb[ssidTag()]),CSTR(_cb[pskTag()]));
    WiFi.mode(WIFI_STA);
    WiFi.enableAP(false); 
    WiFi.setSleep(false);
	WiFi.setAutoReconnect(true);
    WiFi.begin(CSTR(_cb[ssidTag()]),CSTR(_cb[pskTag()]));
}

void H4P_WiFi::_start(){ Serial.printf("H4P_WiFi::_start() 32\n"); WiFi.begin(); };
/*
#if H4P_USE_WFIF_AP
void H4P_WiFi::_coreStart(){ 
    auto mode=WiFi.getMode();
    Serial.printf("H4P_WiFi AP::::_coreStart() 32 mode=%d psk=%s\n",mode,CSTR(WiFi.psk()));
    if(mode==WIFI_AP || WiFi.psk()=="H4") startAP();
}
#else
void H4P_WiFi::_coreStart(){
    Serial.printf(" ESP32 H4P_WiFi::_coreStart()\n");
    if(WiFi.psk()=="H4") h4wifi.HAL_WIFI_startSTA();
}
#endif
*/
void H4P_WiFi::_wifiEvent(WiFiEvent_t event) {
    Serial.printf("ESP32 WIFI EVENT %d\n",event);
    switch(event) {
        case SYSTEM_EVENT_WIFI_READY:
            Serial.printf("SYSTEM_EVENT_WIFI_READY");
			h4.queueFunction([](){ h4wifi._coreStart(); });
            break;
        case SYSTEM_EVENT_STA_LOST_IP:
			h4.queueFunction([](){ h4wifi._lostIP(); });
            break;
		case SYSTEM_EVENT_STA_GOT_IP:
			h4.queueFunction([](){ h4wifi._gotIP(); WiFi.printDiag(Serial); });
			break;
	}
}
#endif
/*
                                                                                                 C O M M O N   F U N C T I O N S
*/
#if H4P_USE_WFIF_AP
void H4P_WiFi::_coreStart(){ 
    auto mode=WiFi.getMode();
    Serial.printf("H4P_WiFi AP::::_coreStart() 32 mode=%d psk=%s\n",mode,CSTR(WiFi.psk()));
    if(mode==WIFI_AP || WiFi.psk()=="H4") startAP();
}
#else
void H4P_WiFi::_coreStart(){
    Serial.printf(" ESP32 H4P_WiFi::_coreStart()\n");
    if(WiFi.psk()=="H4") h4wifi.HAL_WIFI_startSTA();
}
#endif

void H4P_WiFi::_stop(){
    Serial.printf("H4P_WiFi::_stop() common\n");
    h4.cancelSingleton(H4P_TRID_HOTA);
    WiFi.mode(WIFI_OFF);
}

void H4P_WiFi::clear(){ 
    Serial.printf("H4P_WiFi::_clear() common\n");
    // === eraseConfig :)
    WiFi.mode(WIFI_STA);
    WiFi.begin("H4","H4"); 
    show();
	stop();
    HAL_FS.remove(CSTR(string("/"+string(deviceTag()))));
    HAL_FS.remove(CSTR(string("/"+string(nameTag()))));
}

uint32_t H4P_WiFi::_change(vector<string> vs){ return guardString2(vs,[this](string a,string b){ change(a,b); return H4_CMD_OK; }); }

bool H4P_WiFi::_getPersistentValue(string v,string prefix){
    string persistent=replaceAll(H4P_SerialCmd::read("/"+v),"\r\n","");
    _cb[v]=persistent.size() ? persistent:string(prefix)+_cb[chipTag()];
    return persistent.size();
}

void H4P_WiFi::_gotIP(){
    _discoDone=false;
    _cb[ipTag()]=WiFi.localIP().toString().c_str();
    _cb[ssidTag()]=CSTR(WiFi.SSID());
    _cb[pskTag()]=CSTR(WiFi.psk());

    string host=_cb[deviceTag()];
    h4.every(H4WF_OTA_RATE,[](){ ArduinoOTA.handle(); },nullptr,H4P_TRID_HOTA,true);
    HAL_WIFI_setHost(host);

    if(MDNS.begin(CSTR(host))) {
        MDNS.addService(h4Tag(),tcpTag(),666);
        MDNS.addServiceTxt(h4Tag(),tcpTag(),"id",CSTR(_cb[chipTag()]));
        MDNS.addServiceTxt(h4Tag(),tcpTag(),ipTag(),CSTR(_cb[ipTag()]));
    } //else Serial.println("Error starting mDNS");

  	ArduinoOTA.setHostname(CSTR(host));
	ArduinoOTA.setRebootOnSuccess(false);	
	ArduinoOTA.begin(); // matching end???

    H4EVENT("IP=%s",CSTR(_cb[ipTag()]));
    _upHooks();
}

void H4P_WiFi::_hookIn(){
    _cb[chipTag()]=HAL_WIFI_chipID();
    _cb[boardTag()]=replaceAll(H4_BOARD,"ESP8266_","");
    if(!_getPersistentValue(deviceTag(),"H4-")) if(_device!="") _cb[deviceTag()]=_device;
    H4EVENT("Device %s chip %s",CSTR(_cb[deviceTag()]),CSTR(_cb[chipTag()]));
    _getPersistentValue(h4UIvTag(),"NO UI! ");
    WiFi.persistent(true);
    WiFi.onEvent(_wifiEvent);

    h4asws._uiAdd(4,boardTag(),H4P_UI_LABEL);
    h4asws._uiAdd(5,chipTag(),H4P_UI_LABEL);
    h4asws._uiAdd(1,deviceTag(),H4P_UI_LABEL);
    h4asws._uiAdd(6,ipTag(),H4P_UI_LABEL,"",[]{ return _cb[ipTag()]; }); // cos we don't know it yet
    h4asws._uiAdd(7,"H4v",H4P_UI_LABEL,H4_VERSION);
    h4asws._uiAdd(8,h4PvTag(),H4P_UI_LABEL);
#if H4P_USE_WIFI_AP
    _cb[h4UIvTag()]=_cb[h4UIvTag()]+"ap";
#endif
    h4asws._uiAdd(9,h4UIvTag(),H4P_UI_LABEL);
    if(isLoaded(mqttTag())) h4asws._uiAdd(10,"Pangolin Vn",H4P_UI_LABEL,_cb[pmvTag()]);
    if(isLoaded(upnpTag())) {
        h4asws._uiAdd(2,"name",H4P_UI_LABEL,"",[]{ return _cb[nameTag()]; }); // cos we don't know it yet
        h4cmd.addCmd("host2",_subCmd,0,CMDVS(_host2));
    }
}

uint32_t H4P_WiFi::_host(vector<string> vs){
    return guard1(vs,[this](vector<string> vs){
        return ([this](string h){
            host(h); 
            return H4_CMD_OK;
        })(H4PAYLOAD);
    });
}

uint32_t H4P_WiFi::_host2(vector<string> vs){ return guardString2(vs,[this](string a,string b){ setBothNames(a,b); return H4_CMD_OK; }); }

void H4P_WiFi::_lostIP(){
//    H4EVENT("_lostIP() done=%d",_discoDone);
    h4.cancelSingleton(H4P_TRID_HOTA);
    if(!_discoDone){
        _downHooks();
        _discoDone=true;
    }
}

void H4P_WiFi::_setPersistentValue(string n,string v,bool reboot){
    if(_cb[n]!=v){
        H4P_SerialCmd::write("/"+n,v);
        if(reboot) h4reboot();
    }
}

void H4P_WiFi::change(string ssid,string psk){ // add device / name?
    stop();
    _cb[ssidTag()]=ssid;
    _cb[pskTag()]=psk;
    HAL_WIFI_startSTA();
    h4reboot();
}

void H4P_WiFi::setBothNames(const string& hostname,const string& friendly){ 
    if(isLoaded(upnpTag())) _setPersistentValue(nameTag(),friendly,false);
    host(hostname);
}