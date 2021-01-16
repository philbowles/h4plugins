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
#include<H4P_FlasherController.h>
#include<H4P_GPIOManager.h>
#include<H4P_VerboseMessages.h>

STAG(tcp)

void __attribute__((weak)) onUiChange(const string& name,const string& value){ h4UserEvent("onUiChange %s now=%s",CSTR(name),CSTR(value)); }
void __attribute__((weak)) onViewers(){}
void __attribute__((weak)) onNoViewers(){}

/*
                                             H / W - D E P E N D E N T   F U N C T I O N S
*/
#ifdef ARDUINO_ARCH_ESP8266
//
//                                                                                                  ESP8266
//
string H4P_WiFi::HAL_WIFI_chipID(){ return stringFromInt(ESP.getChipId(),"%06X"); }

void H4P_WiFi::HAL_WIFI_disconnect(){ WiFi.disconnect(false); }// prb 32 only: abandon curretn blank attempt or no scan possible

void H4P_WiFi::HAL_WIFI_setHost(const string& host){ WiFi.hostname(CSTR(host)); } // change ????

void H4P_WiFi::HAL_WIFI_startSTA(){
    WiFi.mode(WIFI_STA); // poss lose?
	WiFi.setSleepMode(WIFI_NONE_SLEEP);
	WiFi.setAutoConnect(true);
	WiFi.setAutoReconnect(true);
    WiFi.begin(CSTR(_cb[ssidTag()]),CSTR(_cb[pskTag()]));
}

void H4P_WiFi::_start(){     
    _badSignal();
    _coreStart();
}

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
void H4P_WiFi::HAL_WIFI_disconnect(){ WiFi.disconnect(false,false); }// prb 32 only: abandon curretn blank attempt or no scan possible

void H4P_WiFi::HAL_WIFI_setHost(const string& host){ WiFi.setHostname(CSTR(host)); }

void H4P_WiFi::HAL_WIFI_startSTA(){
    WiFi.setSleep(false);
	WiFi.setAutoReconnect(true);
    WiFi.begin(CSTR(_cb[ssidTag()]),CSTR(_cb[pskTag()]));
}

void H4P_WiFi::_start(){ 
    _badSignal();
    WiFi.begin();
 };

void H4P_WiFi::_wifiEvent(WiFiEvent_t event) {
/*
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
*/
    Serial.printf("ESP32 WIFI EVENT %d\n",event);
    switch(event) {
        case SYSTEM_EVENT_WIFI_READY:
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
#endif
/*
                                                                                                 C O M M O N   F U N C T I O N S
*/
#if H4P_USE_WIFI_AP
    void H4P_WiFi::_coreStart(){ if(_cannotConnectSTA()) _startAP(); }
#else
    void H4P_WiFi::_coreStart(){ if(_cannotConnectSTA()) h4wifi.HAL_WIFI_startSTA(); }
#endif

void H4P_WiFi::_stop(){
    h4.cancelSingleton(H4P_TRID_HOTA);
    WiFi.mode(WIFI_OFF);
}

void H4P_WiFi::clear(){ 
    WiFi.mode(WIFI_STA);
    WiFi.begin("H4","H4"); // // === eraseConfig :) becos it wont allow "",""
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
    signalOff();
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
    _startWebserver();
    _upHooks();
}

void H4P_WiFi::_hookIn(){
    DEPEND(wink);
    _cb[chipTag()]=HAL_WIFI_chipID();
    _cb[boardTag()]=replaceAll(H4_BOARD,"ESP8266_","");
    if(!_getPersistentValue(deviceTag(),"H4-")) if(_device!="") _cb[deviceTag()]=_device;
    H4EVENT("Device %s chip %s",CSTR(_cb[deviceTag()]),CSTR(_cb[chipTag()]));
    _getPersistentValue(h4UITag(),"NO UI! ");
    WiFi.persistent(true);
    WiFi.onEvent(_wifiEvent);

    H4Plugin* p=isLoaded(onofTag());
    if(p) _btp=reinterpret_cast<H4P_BinaryThing*>(p);
    if(isLoaded(gpioTag())) h4cmd.addCmd("gpio",_subCmd,0,CMDVS(_gpio));

    _uiAdd(H4P_UIO_BOARD,boardTag(),H4P_UI_LABEL);
    _uiAdd(H4P_UIO_CHIP,chipTag(),H4P_UI_LABEL);
    _uiAdd(H4P_UIO_DEVICE,deviceTag(),H4P_UI_LABEL);
    _uiAdd(H4P_UIO_IP,ipTag(),H4P_UI_LABEL,"",[]{ return _cb[ipTag()]; }); // cos we don't know it yet
    _uiAdd(H4P_UIO_H4V,h4Tag(),H4P_UI_LABEL,H4_VERSION);
    _uiAdd(H4P_UIO_H4PV,h4pTag(),H4P_UI_LABEL);
#if H4P_USE_WIFI_AP
    _cb[h4UITag()]=_cb[h4UITag()]+"ap";
#endif
    _uiAdd(H4P_UIO_H4UIV,h4UITag(),H4P_UI_LABEL);
}

uint32_t H4P_WiFi::_gpio(vector<string> vs){
    return guard1(vs,[this](vector<string> vs){
        if(H4PAYLOAD=="all") uiAddGPIO();
        else {
            if(!isNumeric(H4PAYLOAD)) return H4_CMD_NOT_NUMERIC;
            uint8_t pin=H4PAYLOAD_INT;
            return uiAddGPIO(pin);
        }
        return H4_CMD_OK;
    });
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
    if(!_discoDone) _stopWebserver();
}

uint32_t H4P_WiFi::_msg(vector<string> vs){
    return guard1(vs,[this](vector<string> vs){
        uiMessage(H4PAYLOAD);
        return H4_CMD_OK;
    });
}

void H4P_WiFi::_setPersistentValue(string n,string v,bool reboot){
    if(_cb[n]!=v){
        H4P_SerialCmd::write("/"+n,v);
        if(reboot) h4reboot();
    }
}

H4_CMD_ERROR H4P_WiFi::__uichgCore(const string& a,const string& b,H4P_FN_UICHANGE chg){
    _sendSSE(CSTR(a),CSTR(b));
    if(chg) chg(b);
    else onUiChange(a,b);
    return H4_CMD_OK;
}

void H4P_WiFi::_uiAdd(uint32_t seq,const string& i,H4P_UI_TYPE t,const string& v,H4P_FN_UITXT f,H4P_FN_UICHANGE c){
    string value=v;
    if(c) value=_cb[i]; // change function implies input / dropdown etc: backed by CB variable
    else if(value=="") value=f ? f():_cb[i];
    _userItems[seq]={i,t,value,f,c};
}

uint32_t H4P_WiFi::_uichg(vector<string> vs){
    dumpvs(vs);
    string payload;
    string extra;
    vector<string> vg;
    switch(vs.size()){
        case 0:
            return H4_CMD_TOO_FEW_PARAMS;
        case 2:
            extra="/"+vs[1];
        case 1:
            payload=vs[0]+extra;
            Serial.printf("UICHG %s\n",CSTR(payload));
            vg=split(payload,",");
            if(vg.size()>2) return H4_CMD_TOO_MANY_PARAMS;
            if(vg.size()<2) return H4_CMD_TOO_FEW_PARAMS;
            for(auto &i:_userItems){
                H4P_UI_ITEM u=i.second;
                if(u.id==vg[0]){
                    if(u.type==H4P_UI_BOOL){
                        if(isNumeric(vg[1])) {
                            if(atoi(CSTR(vg[1])) > 1) return H4_CMD_PAYLOAD_FORMAT;
                        } else return H4_CMD_NOT_NUMERIC;
                    }
                    _cb[vg[0]]=vg[1];
                    return __uichgCore(vg[0],vg[1],u.c);
                }
            }
            return H4_CMD_NAME_UNKNOWN;
        default:
            return H4_CMD_TOO_MANY_PARAMS;
    }
}

void H4P_WiFi::_rest(AsyncWebServerRequest *request){
	h4.queueFunction(bind([this](AsyncWebServerRequest *request){
        H4EVENT("_rest %s",request->client()->remoteIP().toString().c_str());
		string chop=replaceAll(CSTR(request->url()),"/rest/","");
        string msg="";
        uint32_t res=h4cmd._simulatePayload(CSTR(chop),wifiTag());
        if(isLoaded(vmTag())) msg=h4vm.getErrorMessage(res);
        string j="{\"res\":"+stringFromInt(res)+",\"msg\":\""+msg+"\",\"lines\":[";
        string fl;
        if(!res){
            for(auto &l:_lines){
                if(l.back()=='\n') l.pop_back();
                fl+="\""+l+"\",";
            }
            if(fl.size()) fl.pop_back();
        }
        j+=fl+"]}";
        AsyncWebServerResponse *response = request->beginResponse(200, "application/json",CSTR(j));
        response->addHeader("Access-Control-Allow-Origin","*");
        request->send(response);
        _lines.clear();
	},request),nullptr,H4P_TRID_REST);
}

void H4P_WiFi::_sendSSE(const char* name,const char* msg){
    static uint32_t _evtID=0;
    if(_evts && _evts->count()) _evts->send(msg,name,_evtID++);
}

void H4P_WiFi::_startWebserver(){
	reset();
    _evts=new AsyncEventSource("/evt");
    _evts->onConnect([this](AsyncEventSourceClient *client){
        h4.queueFunction([this,client](){
            H4EVENT("SSE Client %08x n=%d T/O=%d nC=%d nUI=%d",client,client->lastId(),H4P_ASWS_EVT_TIMEOUT,_evts->count(),_userItems.size());
            if(_evts->count()==1) onViewers(); //if(_onC) _onC(); // first time only R WE SURE?
            for(auto & i:_userItems) {
                H4P_UI_ITEM u=i.second;
                if(u.f) u.value=u.f(); // auto-sync delayed fields
                _sendSSE("ui",CSTR(string(u.id+","+stringFromInt(u.type)+","+u.value+","+(u.c ? "1":"0" ))));
            }
            h4.repeatWhile([this]{ return _evts->count(); },
                ((H4P_ASWS_EVT_TIMEOUT*3)/4),
                [this]{ },
                [this]{ onNoViewers(); },
                H4P_TRID_SSET,true
            );
        });
    });
    addHandler(_evts);

    on("/",HTTP_GET, [this](AsyncWebServerRequest *request){
        H4EVENT("Root %s",request->client()->remoteIP().toString().c_str());
        request->send(HAL_FS,"/sta.htm",String(),false,_aswsReplace);
    });

	on("/rest",HTTP_GET,[this](AsyncWebServerRequest *request){ _rest(request); });

    serveStatic("/", HAL_FS, "/").setCacheControl("max-age=31536000");
    begin();
}

void H4P_WiFi::_stopWebserver(){ 
    _badSignal();
    end();
    _downHooks();
    _discoDone=true;
}

String H4P_WiFi::_aswsReplace(const String& var){
    string v=CSTR(var);
    return _cb.count(v) ? String(CSTR(_cb[v])):"?";
}

void H4P_WiFi::change(string ssid,string psk){ // add device / name?
    stop();
    _cb[ssidTag()]=ssid;
    _cb[pskTag()]=psk;
    HAL_WIFI_startSTA();
    h4reboot();
}

void H4P_WiFi::signal(const char* pattern,uint32_t timebase){ h4fc.flashMorse(pattern,timebase,H4P_SIGNAL_LED,H4P_SIGNAL_SENSE); }

void H4P_WiFi::signalOff(){ h4fc.stopLED(H4P_SIGNAL_LED); }

void H4P_WiFi::uiAddDropdown(const string& name,H4P_CONFIG_BLOCK options,H4P_FN_UICHANGE onChange){
    string opts="";
    for(auto const& o:options) opts+=o.first+"="+o.second+",";
    opts.pop_back();
    _uiAdd(_seq++,name,H4P_UI_DROPDOWN,opts,nullptr,onChange); // optimise
}

void H4P_WiFi::uiAddGPIO(){ if(isLoaded(gpioTag())) for(auto const& p:H4P_GPIOManager::pins) uiAddGPIO(p.first); }

H4_CMD_ERROR H4P_WiFi::uiAddGPIO(uint8_t pin){
    if(isLoaded(gpioTag())){
        H4GPIOPin*  p;
        if(p=h4gm.isManaged(pin)) {
            char buf[32];
            snprintf(buf,31,"GP%02d(Type%02d)",pin,p->style);
            string name(buf);
            H4GM_FN_EVENT f=p->onEvent;
            p->onEvent=[this,name,f](H4GPIOPin* hp){
                _sendSSE(CSTR(name),hp->logicalRead() ? "1":"0");
                if(f) f(hp);
            };
            _uiAdd(H4P_UIO_GPIO+pin,name,H4P_UI_BOOL,"",[p]{ return p->logicalRead() ? "1":"0"; });
            return H4_CMD_OK;
        } 
        return H4_CMD_OUT_OF_BOUNDS;
    }
    return H4_CMD_NOT_NOW;
}

void H4P_WiFi::uiAddInput(const string& name,const string& value,H4P_FN_UICHANGE onChange){
    if(value!="") _cb[name]=value;
    _uiAdd(_seq++,name,H4P_UI_INPUT,_cb[name],nullptr,onChange); 
}

void H4P_WiFi::uiSync(){
    if(_evts && _evts->count()){
        for(auto &i:_userItems){
            H4P_UI_ITEM u=i.second;
            if(u.f) { // refakta syncCore
                if(u.f) u.value=u.f();
                _sendSSE(CSTR(u.id),CSTR(u.value));
            }
        }
    }
}