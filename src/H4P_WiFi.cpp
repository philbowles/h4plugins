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
#include<H4P_WiFi.h>
#include<H4P_PinMachine.h>

H4P_UI_LIST         h4pUserItems;
vector<string>      h4pUIorder;
/*
                                             H / W - D E P E N D E N T   F U N C T I O N S
*/
#ifdef ARDUINO_ARCH_ESP8266
//
//                                                                                                  ESP8266
//
string H4P_WiFi::HAL_WIFI_chipID(){ return stringFromInt(ESP.getChipId(),"%06X"); }

void H4P_WiFi::HAL_WIFI_disconnect(){ WiFi.disconnect(false); }

void H4P_WiFi::HAL_WIFI_setHost(const string& host){ WiFi.hostname(CSTR(host)); }

void H4P_WiFi::HAL_WIFI_startSTA(){
    WiFi.mode(WIFI_STA); // poss lose?
	WiFi.setSleepMode(WIFI_NONE_SLEEP);
	WiFi.setAutoConnect(true);
	WiFi.setAutoReconnect(true);
    WiFi.begin(CSTR(h4p[ssidTag()]),CSTR(h4p[pskTag()]));
}

void H4P_WiFi::svcUp(){
    _signalBad();
    _coreStart();
}
/*#typedef enum WiFiEvent 
{
    WIFI_EVENT_STAMODE_CONNECTED = 0,
    WIFI_EVENT_STAMODE_DISCONNECTED, 1 
    WIFI_EVENT_STAMODE_AUTHMODE_CHANGE, 2
    WIFI_EVENT_STAMODE_GOT_IP, 3
    WIFI_EVENT_STAMODE_DHCP_TIMEOUT, 4
    WIFI_EVENT_SOFTAPMODE_STACONNECTED, 5
    WIFI_EVENT_SOFTAPMODE_STADISCONNECTED, 6
    WIFI_EVENT_SOFTAPMODE_PROBEREQRECVED,7
    WIFI_EVENT_MODE_CHANGE, 8
    WIFI_EVENT_SOFTAPMODE_DISTRIBUTE_STA_IP,9
    WIFI_EVENT_MAX,10
    WIFI_EVENT_ANY = WIFI_EVENT_MAX,
} WiFiEvent_t;
*/

void H4P_WiFi::_wifiEvent(WiFiEvent_t event) {
//    Serial.printf("*****************************8 WiFi EVENT %d\n",event);
    switch(event) {
        case WIFI_EVENT_STAMODE_DISCONNECTED:
			h4.queueFunction([](){ h4puncheckedcall<H4P_WiFi>(wifiTag())->_lostIP(); });
            break;    
		case WIFI_EVENT_STAMODE_GOT_IP:
			h4.queueFunction([](){ h4puncheckedcall<H4P_WiFi>(wifiTag())->_gotIP(); });
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

void H4P_WiFi::HAL_WIFI_disconnect(){ WiFi.disconnect(false,false); }

void H4P_WiFi::HAL_WIFI_setHost(const string& host){ WiFi.setHostname(CSTR(host)); }

void H4P_WiFi::HAL_WIFI_startSTA(){
    WiFi.setSleep(false);
	WiFi.setAutoReconnect(true);
    WiFi.begin(CSTR(h4p[ssidTag()]),CSTR(h4p[pskTag()]));
}

void H4P_WiFi::svcUp(){ 
    _signalBad();
    WiFi.begin();
};

void H4P_WiFi::_wifiEvent(WiFiEvent_t event) {
    switch(event) {
        case SYSTEM_EVENT_WIFI_READY:
			h4.queueFunction([](){ h4puncheckedcall<H4P_WiFi>(wifiTag())->_coreStart(); });
            break;
        case SYSTEM_EVENT_STA_STOP:
        case SYSTEM_EVENT_STA_LOST_IP:
			h4.queueFunction([](){ h4puncheckedcall<H4P_WiFi>(wifiTag())->_lostIP(); });
            break;
		case SYSTEM_EVENT_STA_GOT_IP:
			h4.queueFunction([](){ h4puncheckedcall<H4P_WiFi>(wifiTag())->_gotIP(); });
			break;
	}
}

#endif

void H4P_WiFi::_coreStart(){
//    WiFi.printDiag(Serial); 
#if H4P_USE_WIFI_AP
//    Serial.printf("AP MODE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    if(!_dns53){
        if(_cannotConnectSTA()) _startAP();
        else if(WiFi.getMode()==WIFI_OFF) h4puncheckedcall<H4P_WiFi>(wifiTag())->HAL_WIFI_startSTA();
    }
#else
    if(_cannotConnectSTA() || WiFi.getMode()==WIFI_OFF) h4puncheckedcall<H4P_WiFi>(wifiTag())->HAL_WIFI_startSTA();
#endif
}

String H4P_WiFi::_aswsReplace(const String& var){
    string v=CSTR(var);
    return h4p.gvExists(v) ? String(CSTR(h4p[v])):"?";
}

uint32_t H4P_WiFi::_change(vector<string> vs){ return _guardString2(vs,[this](string a,string b){ change(a,b); return H4_CMD_OK; }); }

void H4P_WiFi::_clear(){
//    WiFi.printDiag(Serial);
    H4Service::svcDown();
    WiFi.onEvent([](WiFiEvent_t event){});
    WiFi.mode(WIFI_STA);
    WiFi.begin(h4Tag(),h4Tag()); // // === eraseConfig :) becos it wont allow "",""
    h4p.gvErase({deviceTag(),nameTag()}); // LOSE THIS??
}

void H4P_WiFi::_gotIP(){
//    if(!_running){        
    _signalOff();
    _discoDone=false;
    h4p[ipTag()]=WiFi.localIP().toString().c_str();
    h4p[ssidTag()]=CSTR(WiFi.SSID());
    h4p[pskTag()]=CSTR(WiFi.psk());

    string host=h4p[deviceTag()];
    h4.every(H4WF_OTA_RATE,[](){ ArduinoOTA.handle(); },nullptr,H4P_TRID_HOTA,true);
    HAL_WIFI_setHost(host);

    if(MDNS.begin(CSTR(host))) { // confirm if still needed 2) esp32 _ version
        MDNS.addService("http",tcpTag(),H4P_WEBSERVER_PORT);
        MDNS.addService(h4Tag(),tcpTag(),666);
        MDNS.addServiceTxt(h4Tag(),tcpTag(),"id",CSTR(h4p[chipTag()]));
        MDNS.addServiceTxt(h4Tag(),tcpTag(),ipTag(),CSTR(h4p[ipTag()]));
    }

    ArduinoOTA.setHostname(CSTR(host));
    ArduinoOTA.setRebootOnSuccess(false);	
    ArduinoOTA.begin(); // matching end???

    SYSINFO("IP=%s",CSTR(h4p[ipTag()]));
    _startWebserver();
    H4Service::svcUp();
}

void H4P_WiFi::__uiAdd(const string& msg) {
    vector<string> m=split(msg,",");
    _uiAdd(m[0],static_cast<H4P_UI_TYPE>(STOI(m[1])),m[3],m[2],STOI(m[4]));    
}

void H4P_WiFi::_clearUI(){ 
    h4pUserItems.clear();
    h4pUIorder.clear();
    h4pUIorder.shrink_to_fit();
}

void H4P_WiFi::_commonStartup(){
#ifdef LED_BUILTIN
    new h4pOutput(LED_BUILTIN,H4P_ASSUMED_SENSE,OFF,H4P_ASSUMED_COLOR);
    require<H4P_PinMachine>(gpioTag());
    require<H4P_Signaller>(winkTag());
#endif
    _addLocals({
        {_me,           { H4PC_H4, _pid, nullptr}},
        {changeTag(),   { _pid, 0, CMDVS(_change)}},
        {"msg",         { _pid, 0, CMDVS(_msg)}},
    });
}

void H4P_WiFi::_defaultSync(const string& svc,const string& msg) {
    if(h4pUserItems.count(svc)) {
        Serial.printf("H4P_WiFi::GVC DFLT SYNC %s %s\n",CSTR(svc),CSTR(msg));
        string sync;
        switch(h4pUserItems[svc].type){
            case H4P_UI_DROPDOWN:
                sync=h4pUserItems[svc].f();
                break;
            case H4P_UI_IMGBTN:
                if(svc==stateTag()) sync=msg;
                else {
                    Serial.printf("%s No AUTO-SYNC for buttons\n",CSTR(svc));
                    return;
                }
            default:
                sync=msg;
        }
        _sendSSE(svc,sync);
    }
}

void H4P_WiFi::_handleEvent(const string& svc,H4PE_TYPE t,const string& msg) {
    switch(t){
        case H4PE_UIMSG:
            uiMessage("%s",CSTR(msg));
            break;
        case H4PE_FACTORY:
            _clear();
            break;
        case H4PE_UIADD:
            __uiAdd(msg);
            break;
        case H4PE_GPIO:
        case H4PE_UISYNC:
            //_defaultSync(svc,msg);
            if(h4pUserItems.count(svc)) _sendSSE(svc,msg);
            break;
        case H4PE_GVCHANGE:
#if H4P_USE_WIFI_AP
            if(svc == GoTag() && STOI(msg)) {
//                Serial.printf("DO NOT COLLECT £200 *%s*\n",CSTR(msg));
                HAL_WIFI_startSTA();
                h4.once(1000,[]{ h4pevent(h4pSrc,H4PE_REBOOT,GoTag()); });
//                Serial.printf("YOU'RE GOIN' DAAAAAAAAAAAAAAAAAHN\n");
                return;
            }
#endif
            if(svc == deviceTag() && _running) h4pevent(h4pSrc,H4PE_REBOOT,svc);
            _defaultSync(svc,msg);
        default:
//            Serial.printf("H4P_WiFi::HEVT %s %s %s\n",CSTR(svc),CSTR(h4pGetEventName(t)),CSTR(msg));
            break;
    }
}

void H4P_WiFi::_init(){
//    Serial.printf("********************* H4P_WiFi::_init ********** prevent double dip\n");
    WiFi.persistent(true);
    WiFi.onEvent(_wifiEvent);
    
    h4p.gvSetstring(chipTag(),HAL_WIFI_chipID());
    h4p.gvSetstring(boardTag(),replaceAll(H4_BOARD,"ESP8266_",""));

    if(h4p[deviceTag()]=="") h4p[deviceTag()]=string("H4-").append(HAL_WIFI_chipID());
    h4p.gvSetstring(h4UITag(),h4p.read(string("/").append(h4UITag())));

    if(h4p[h4UITag()]=="") SYSWARN("NO FS: webUI will not start","");
    SYSINFO("Device: %s Chip: %s UI v%s",CSTR(h4p[deviceTag()]),CSTR(h4p[chipTag()]),CSTR(h4p[h4UITag()]));

//    h4p.gvSetstring(ipTag(),"",false);
    h4p.gvSave(deviceTag());
}

void H4P_WiFi::_lostIP(){
    h4.cancelSingleton(H4P_TRID_HOTA);
    if(!_discoDone) _stopWebserver();
}

uint32_t H4P_WiFi::_msg(vector<string> vs){
    return _guard1(vs,[this](vector<string> vs){
        uiMessage(H4PAYLOAD);
        return H4_CMD_OK;
    });
}

void H4P_WiFi::_rest(AsyncWebServerRequest *request){
	h4.queueFunction([=](){
        XLOG("_rest %s",request->client()->remoteIP().toString().c_str());
		string chop=replaceAll(CSTR(request->url()),"/rest/","");
        string msg="";
        uint32_t res=h4puncheckedcall<H4P_SerialCmd>(cmdTag())->_simulatePayload(CSTR(chop),wifiTag());
        msg=h4pGetErrorMessage(res);
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
	},nullptr,H4P_TRID_REST);
}

void H4P_WiFi::_sendSSE(const string& name,const string& msg){
    static bool bakov=false;

    if(_evts && _evts->count()) {
        auto fh=_HAL_freeHeap();
        if(fh > H4P_THROTTLE_LO){
            if(bakov && (fh > H4P_THROTTLE_HI)){
                SYSINFO("SSE BACKOFF RECOVERY H=%lu nQ=%d",fh,_evts->avgPacketsWaiting());
                bakov=false;
                for(auto const& ui:h4pUserItems){
//                    Serial.printf("REALIGN %s => %s\n",CSTR(ui.first),CSTR(ui.second.f()));
                    _evts->send(CSTR(ui.second.f()),CSTR(ui.first),_evtID++); // hook to sse event q size
                }
            }
            _evts->send(CSTR(msg),CSTR(name),_evtID++); // hook to sse event q size
        } 
        else {
            bakov=true;
            SYSWARN("SSE BACKOFF H=%lu nQ=%d",fh,_evts->avgPacketsWaiting());
        }
    }
}

void H4P_WiFi::_startWebserver(){
	reset();
    _evts=new AsyncEventSource("/evt");
    _evts->onConnect([this](AsyncEventSourceClient *client){
        if(client->lastId() > _evtID) client->close();
        else {
            h4.queueFunction([this,client](){
                if(_evts->count()==1) {
                #if H4P_USE_WIFI_AP
                    if(WiFi.getMode()==WIFI_AP) {
                        _uiAdd(chipTag(),H4P_UI_TEXT,"s"); // clumsy, don't like
                        _uiAdd(deviceTag(),H4P_UI_INPUT,"s");
                    }
                    else {
                        _clearUI();
                        _uiAdd(chipTag(),H4P_UI_TEXT,"s");
                        _uiAdd(deviceTag(),H4P_UI_TEXT,"s");
                        _uiAdd(boardTag(),H4P_UI_TEXT,"s");
                        _uiAdd(h4Tag(),H4P_UI_TEXT,"s",H4_VERSION);
                        _uiAdd(H4PTag(),H4P_UI_TEXT,"s");
                        _uiAdd(h4UITag(),H4P_UI_TEXT,"s");
                        _uiAdd(NBootsTag(),H4P_UI_TEXT,"s");
                        _uiAdd(ipTag(),H4P_UI_TEXT,"s"); // cos we don't know it yet
                    }
                #else
                    _clearUI();
                    _uiAdd(chipTag(),H4P_UI_TEXT,"s");
                    _uiAdd(deviceTag(),H4P_UI_TEXT,"s");
                    _uiAdd(boardTag(),H4P_UI_TEXT,"s");
                    _uiAdd(h4Tag(),H4P_UI_TEXT,"s",H4_VERSION);
                    _uiAdd(H4PTag(),H4P_UI_TEXT,"s");
                    _uiAdd(h4UITag(),H4P_UI_TEXT,"s");
                    _uiAdd(NBootsTag(),H4P_UI_TEXT,"s");
                    _uiAdd(ipTag(),H4P_UI_TEXT,"s"); // cos we don't know it yet
                #endif
                    h4psysevent("viewers",H4PE_VIEWERS,"%d",WiFi.getMode());
                    h4pUIorder.shrink_to_fit();
                }
                for(auto const& ui:h4pUIorder){
                    auto i=h4pUserItems[ui];
//                    _sendSSE("ui",CSTR(string(ui+","+stringFromInt(i.type)+","+i.f()+",1,"+stringFromInt(i.color)+","+i.h)));
                    _sendSSE("ui",CSTR(string(ui+","+stringFromInt(i.type)+","+i.h+",0,"+stringFromInt(i.color)+","+i.f())));
                }
                h4.repeatWhile([this]{ return _evts->count(); },
                    ((H4WF_EVT_TIMEOUT*3)/4),
                    [this]{ },
                    [this]{ h4psysevent("viewers",H4PE_VIEWERS,"%d",0); },
                    H4P_TRID_SSET,true
                );
            });
        }
    });
    addHandler(_evts);

    on("/",HTTP_GET, [this](AsyncWebServerRequest *request){
        XLOG("Root %s",request->client()->remoteIP().toString().c_str());
        request->send(HAL_FS,"/sta.htm",String(),false,_aswsReplace);
    });

	on("/rest",HTTP_GET,[this](AsyncWebServerRequest *request){ _rest(request); });

    serveStatic("/", HAL_FS, "/").setCacheControl("max-age=31536000");
    begin();
}

void H4P_WiFi::_stopWebserver(){ 
    end();
    _discoDone=true;
    _signalBad();
    _clearUI();
    svcDown();
}

void H4P_WiFi::_uiAdd(const string& name,H4P_UI_TYPE t,const string& h,const string& value,uint8_t color){
    function<string(void)>  f;
    string v=value;
    switch(t){
        case H4P_UI_GPIO:
            f=[=](){ return stringFromInt(H4P_PinMachine::logicalRead(STOI(name))); };
            break;
        case H4P_UI_DROPDOWN:
            if(!h4p.gvExists(name)) h4p.gvSetstring(name,"");
            f=[=]{ return value; };
            break;
        default:
//            Serial.printf("DEF UIA %s t=%d h=%s c=%d v=*%s* (%d)\n",CSTR(name),t,CSTR(h),color,CSTR(value),value.size());
            if(v.size()) f=[=](){ return v; };
            else {
//                Serial.printf("NOVALU %s GLOB=%d\n",CSTR(name),h4p.gvExists(name));
                if(!h4p.gvExists(name)) h4p.gvSetstring(name,value,false);
//                Serial.printf("VALU %s=%s\n",CSTR(name),CSTR(h4p[name]));
                f=[=](){ return h4p[name]; };
            }
    }
    h4pUIorder.push_back(name);
    h4pUserItems[name]={t,f,color,h};
}
/*

        PUBLICS

*/
void H4P_WiFi::change(string ssid,string psk){ // add device / name?
    svcDown();
    h4p[ssidTag()]=ssid;
    h4p[pskTag()]=psk;
    HAL_WIFI_startSTA();
//    MDNS.notifyAPChange(); // ?
#if H4P_WIFI_USE_AP
    if(_dns53) QEVENT(H4PE_REBOOT);
#endif
}

#if H4P_LOG_MESSAGES
void H4P_WiFi::info() { 
    H4Service::info();
//    WiFi.printDiag(Serial);
    reply(" Device %s Mode=%d Status: %d IP=%s",CSTR(h4p[deviceTag()]),WiFi.getMode(),WiFi.status(),WiFi.localIP().toString().c_str());
    reply(" SSID %s PSK=%s",CSTR(WiFi.SSID()),CSTR(WiFi.psk()));
    #if H4P_ASSUMED_LED
        reply(" Signal Pin GPIO%d active=%d",H4P_ASSUMED_LED,H4P_ASSUMED_SENSE);
    #else
        reply(" ** NO Signal Pin! **");
    #endif
}
#endif

void H4P_WiFi::svcDown(){
    h4.cancelSingleton(H4P_TRID_HOTA);
    WiFi.mode(WIFI_OFF);
    H4Service::svcDown();
}

void H4P_WiFi::uiAddDropdown(const string& name,H4P_NVP_MAP options,const string& section){ _uiAdd(name,H4P_UI_DROPDOWN,section,flattenMap(options)); }