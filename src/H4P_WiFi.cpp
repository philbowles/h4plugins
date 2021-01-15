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
//    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
	WiFi.setAutoReconnect(true);
    Serial.printf("BEGIN 32 %s %s\n",CSTR(_cb[ssidTag()]),CSTR(_cb[pskTag()]));
    WiFi.begin(CSTR(_cb[ssidTag()]),CSTR(_cb[pskTag()]));
    WiFi.printDiag(Serial);
}

void H4P_WiFi::_start(){ 
    Serial.printf("H4P_WiFi::_start() 32 initial mode is %d\n",WiFi.getMode()); 
    WiFi.begin();
    WiFi.printDiag(Serial); // need this just to see what stae we are in! Nuts, I know!
};

void H4P_WiFi::_wifiEvent(WiFiEvent_t event) {

    Serial.printf("[WiFi-event] event: %d\n", event);

    switch (event) {
        case SYSTEM_EVENT_WIFI_READY: 
            Serial.println("WiFi interface ready");
			h4.queueFunction([](){ h4wifi._coreStart(); });
            break;
        case SYSTEM_EVENT_SCAN_DONE:
            Serial.println("Completed scan for access points");
            break;
        case SYSTEM_EVENT_STA_START:
            Serial.println("WiFi client started");
            break;
        case SYSTEM_EVENT_STA_STOP:
            Serial.println("WiFi clients stopped");
            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            Serial.println("Connected to access point");
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            Serial.println("Disconnected from WiFi access point");
            break;
        case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
            Serial.println("Authentication mode of access point has changed");
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            Serial.print("Obtained IP address: ");
            Serial.println(WiFi.localIP());
			h4.queueFunction([](){ h4wifi._gotIP(); });
            break;
        case SYSTEM_EVENT_STA_LOST_IP:
            Serial.println("Lost IP address and IP address is reset to 0");
			h4.queueFunction([](){ h4wifi._lostIP(); });
            break;
        case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
            Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_FAILED:
            Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
            Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_PIN:
            Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
            break;
        case SYSTEM_EVENT_AP_START:
            Serial.println("WiFi access point started");
            break;
        case SYSTEM_EVENT_AP_STOP:
            Serial.println("WiFi access point  stopped");
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            Serial.println("Client connected");
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            Serial.println("Client disconnected");
            break;
        case SYSTEM_EVENT_AP_STAIPASSIGNED:
            Serial.println("Assigned IP address to client");
            break;
        case SYSTEM_EVENT_AP_PROBEREQRECVED:
            Serial.println("Received probe request");
            break;
        case SYSTEM_EVENT_GOT_IP6:
            Serial.println("IPv6 is preferred");
            break;
        case SYSTEM_EVENT_ETH_START:
            Serial.println("Ethernet started");
            break;
        case SYSTEM_EVENT_ETH_STOP:
            Serial.println("Ethernet stopped");
            break;
        case SYSTEM_EVENT_ETH_CONNECTED:
            Serial.println("Ethernet connected");
            break;
        case SYSTEM_EVENT_ETH_DISCONNECTED:
            Serial.println("Ethernet disconnected");
            break;
        case SYSTEM_EVENT_ETH_GOT_IP:
            Serial.println("Obtained IP address");
            break;
        default: break;
    }
}
/*
    Serial.printf("ESP32 WIFI EVENT %d\n",event);
    switch(event) {
        case SYSTEM_EVENT_WIFI_READY:
            Serial.printf("SYSTEM_EVENT_WIFI_READY\n");
			h4.queueFunction([](){ h4wifi._coreStart(); });
            break;
        case SYSTEM_EVENT_STA_LOST_IP:
			h4.queueFunction([](){ h4wifi._lostIP(); });
            break;
		case SYSTEM_EVENT_STA_GOT_IP:
			h4.queueFunction([](){ h4wifi._gotIP(); });
			break;
	}
}
*/
#endif
/*
                                                                                                 C O M M O N   F U N C T I O N S
*/
#if H4P_USE_WIFI_AP
void H4P_WiFi::_coreStart(){ 
    auto mode=WiFi.getMode();
    Serial.printf("H4P_WiFi AP::_coreStart() 32 mode=%d ssid=%s psk=%s\n",mode,CSTR(WiFi.SSID()),CSTR(WiFi.psk()));
    if(WiFi.psk()=="H4"|| WiFi.psk()=="") startAP();
    else {
//        _cb[ssidTag()]=CSTR(WiFi.SSID());
//        _cb[pskTag()]=CSTR(WiFi.psk());
//        h4wifi.HAL_WIFI_startSTA();
    }
}
#else
void H4P_WiFi::_coreStart(){
    Serial.printf("COMMON H4P_WiFi::_coreStart()\n");
    if(WiFi.psk()=="H4" || WiFi.psk()=="") h4wifi.HAL_WIFI_startSTA();
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
    _wipe({deviceTag(),nameTag()});
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
    _getPersistentValue(h4UITag(),"NO UI! ");
    WiFi.persistent(true);
    WiFi.onEvent(_wifiEvent);

    h4asws._uiAdd(H4P_UIO_BOARD,boardTag(),H4P_UI_LABEL);
    h4asws._uiAdd(H4P_UIO_CHIP,chipTag(),H4P_UI_LABEL);
    h4asws._uiAdd(H4P_UIO_DEVICE,deviceTag(),H4P_UI_LABEL);
    h4asws._uiAdd(H4P_UIO_IP,ipTag(),H4P_UI_LABEL,"",[]{ return _cb[ipTag()]; }); // cos we don't know it yet
    h4asws._uiAdd(H4P_UIO_H4V,h4Tag(),H4P_UI_LABEL,H4_VERSION);
    h4asws._uiAdd(H4P_UIO_H4PV,h4pTag(),H4P_UI_LABEL);
#if H4P_USE_WIFI_AP
    _cb[h4UITag()]=_cb[h4UITag()]+"ap";
#endif
    h4asws._uiAdd(H4P_UIO_H4UIV,h4UITag(),H4P_UI_LABEL);
}

uint32_t H4P_WiFi::_host(vector<string> vs){
    return guard1(vs,[this](vector<string> vs){
        return ([this](string h){
            host(h); 
            return H4_CMD_OK;
        })(H4PAYLOAD);
    });
}

void H4P_WiFi::_lostIP(){
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
