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
#include<H4P_Signaller.h>

H4P_UI_LIST                     h4pUserItems;
std::vector<std::string>        h4pUIorder;
/*
                                             H / W - D E P E N D E N T   F U N C T I O N S
*/
#ifdef ARDUINO_ARCH_ESP8266
//
//                                                                                                  ESP8266
//
void H4P_WiFi::HAL_WIFI_disconnect(){ WiFi.disconnect(false); }

void H4P_WiFi::HAL_WIFI_setHost(const std::string& host){ WiFi.hostname(CSTR(host)); }

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
void H4P_WiFi::HAL_WIFI_disconnect(){ WiFi.disconnect(false,false); }

void H4P_WiFi::HAL_WIFI_setHost(const std::string& host){ WiFi.setHostname(CSTR(host)); }

void H4P_WiFi::HAL_WIFI_startSTA(){
    WiFi.setSleep(false);
	WiFi.setAutoReconnect(true);
    WiFi.begin(CSTR(h4p[ssidTag()]),CSTR(h4p[pskTag()]));
}

void H4P_WiFi::svcUp(){ 
    _signalBad();
    WiFi.begin();
};
//WIFI_EVENT_STA_DISCONNECTED
void H4P_WiFi::_wifiEvent(WiFiEvent_t event) {
    switch(event) {
        case SYSTEM_EVENT_WIFI_READY:
			h4.queueFunction([](){ h4puncheckedcall<H4P_WiFi>(wifiTag())->_coreStart(); });
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
			h4.queueFunction([](){ h4puncheckedcall<H4P_WiFi>(wifiTag())->_lostIP(); });
            break;
		case SYSTEM_EVENT_STA_GOT_IP:
			h4.queueFunction([](){ h4puncheckedcall<H4P_WiFi>(wifiTag())->_gotIP(); });
			break;
	}
}

#endif

void H4P_WiFi::_coreStart(){
#if H4P_USE_WIFI_AP
    if(!_dns53){
        if(_cannotConnectSTA()) _startAP();
        else if(WiFi.getMode()==WIFI_OFF) h4puncheckedcall<H4P_WiFi>(wifiTag())->HAL_WIFI_startSTA();
    }
#else
    if(_cannotConnectSTA() || WiFi.getMode()==WIFI_OFF) h4puncheckedcall<H4P_WiFi>(wifiTag())->HAL_WIFI_startSTA();
#endif
}
//
//              common
//
void H4P_WiFi::__uiAdd(const std::string& msg) {
    std::vector<std::string> m=split(msg,",");
    _uiAdd(m[0],static_cast<H4P_UI_TYPE>(STOI(m[1])),m[3],m[2],STOI(m[4]));    
}

String H4P_WiFi::_aswsReplace(const String& var){
    std::string v=CSTR(var);
    return h4p.gvExists(v) ? String(CSTR(h4p[v])):"?";
}

uint32_t H4P_WiFi::_change(std::vector<std::string> vs){ return _guardString2(vs,[this](std::string a,std::string b){ change(a,b); return H4_CMD_OK; }); }

void H4P_WiFi::_clearUI(){ 
    h4pUserItems.clear();
    h4pUIorder.clear();
    h4pUIorder.shrink_to_fit();
}

void H4P_WiFi::_commonStartup(){
#ifdef LED_BUILTIN
    require<H4P_Signaller>(winkTag());
#endif
    if(h4p[deviceTag()]=="") h4p[deviceTag()]=std::string("H4-").append(h4p[chipTag()]);
    XLOG("Device: %s Chip: %s",CSTR(h4p[deviceTag()]),CSTR(h4p[chipTag()]));
    _addLocals({
        {_me,           { H4PC_H4, _pid, nullptr}},
        {changeTag(),   { _pid, 0, CMDVS(_change)}},
        {"msg",         { _pid, 0, CMDVS(_msg)}},
    });
}

void H4P_WiFi::_defaultSync(const std::string& svc,const std::string& msg) {
    if(h4pUserItems.count(svc)) {
        std::string sync;
        switch(h4pUserItems[svc].type){
            case H4P_UI_DROPDOWN:
                sync=h4pUserItems[svc].f();
                break;
            case H4P_UI_IMGBTN:
                if(svc==stateTag()) sync=msg;
                else return;
            default:
                sync=msg;
        }
        _sendSSE(svc,sync);
    }
}

void H4P_WiFi::_gotIP(){
    _signalOff();
    _discoDone=false;
    h4p[ipTag()]=WiFi.localIP().toString().c_str();
    h4p[ssidTag()]=CSTR(WiFi.SSID());
    h4p[pskTag()]=CSTR(WiFi.psk());

    std::string host=h4p[deviceTag()];
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
    ArduinoOTA.begin();

    SYSINFO("IP=%s",CSTR(h4p[ipTag()]));
    _startWebserver();
    H4Service::svcUp();
}

void H4P_WiFi::_handleEvent(const std::string& svc,H4PE_TYPE t,const std::string& msg) {
    switch(t){
        case H4PE_UIMSG:
            uiMessage("%s",CSTR(msg));
            break;
        case H4PE_FACTORY:
            WiFi.mode(WIFI_STA);
            WiFi.begin(h4Tag(),h4Tag()); // // === eraseConfig :) becos it wont allow "",""
            break;
        case H4PE_UIADD:
            __uiAdd(msg);
            break;
        case H4PE_GPIO:
        case H4PE_UISYNC:
            if(h4pUserItems.count(svc)) _sendSSE(svc,msg);
            break;
        case H4PE_GVCHANGE:
#if H4P_USE_WIFI_AP
            if(svc == GoTag() && STOI(msg)) {
                HAL_WIFI_startSTA();
                h4.once(1000,[]{ h4pevent(h4pSrc,H4PE_REBOOT,GoTag()); }); // P for holdoff value?
                return;
            }
#endif
            if(svc == ssidTag() && _running) _restart();
            if(svc == deviceTag() && _running) h4pevent(h4pSrc,H4PE_REBOOT,svc);
            _defaultSync(svc,msg);
        default:
            break;
    }
}

void H4P_WiFi::_init(){
    WiFi.persistent(true);
    WiFi.onEvent(_wifiEvent);
}

void H4P_WiFi::_lostIP(){
    h4.cancelSingleton(H4P_TRID_HOTA);
    if(!_discoDone) {
        _coreStart(); // ESP32 is well and truly fucked
        _stopWebserver();
    }
}

uint32_t H4P_WiFi::_msg(std::vector<std::string> vs){
    return _guard1(vs,[this](std::vector<std::string> vs){
        uiMessage(H4PAYLOAD);
        return H4_CMD_OK;
    });
}

void H4P_WiFi::_rest(AsyncWebServerRequest *request){
	h4.queueFunction([=](){
        XLOG("_rest %s",request->client()->remoteIP().toString().c_str());
		std::string chop=replaceAll(CSTR(request->url()),"/rest/","");
        std::string msg="";
        uint32_t res=h4puncheckedcall<H4P_SerialCmd>(cmdTag())->_simulatePayload(CSTR(chop),wifiTag());
        msg=h4pGetErrorMessage(res);
        std::string j="{\"res\":"+stringFromInt(res)+",\"msg\":\""+msg+"\",\"lines\":[";
        std::string fl;
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

void H4P_WiFi::_restart(){
    svcDown();
    HAL_WIFI_startSTA();
}

void H4P_WiFi::_sendSSE(const std::string& name,const std::string& msg){
    static bool bakov=false;

    if(_evts && _evts->count()) {
        auto fh=_HAL_freeHeap();
        if(fh > H4P_THROTTLE_LO){
            if(bakov && (fh > H4P_THROTTLE_HI)){
                SYSINFO("SSE BACKOFF RECOVERY H=%lu nQ=%d",fh,_evts->avgPacketsWaiting());
                bakov=false;
                for(auto const& ui:h4pUserItems) _evts->send(CSTR(ui.second.f()),CSTR(ui.first),_evtID++); // hook to sse event q size
            }
            _evts->send(CSTR(msg),CSTR(name),_evtID++); // hook to sse event q size
        } 
        else {
            bakov=true;
            SYSWARN("SSE BACKOFF H=%lu nQ=%d",fh,_evts->avgPacketsWaiting());
        }
    }
}

void H4P_WiFi::_signalBad(){ 
    h4p[ipTag()]="";
    H4P_Signaller::signal(H4P_SIG_MORSE,"...   ---   ...   ,150");
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
                        _uiAdd(h4pTag(),H4P_UI_TEXT,"s",H4P_VERSION);
                        _uiAdd(chipTag(),H4P_UI_TEXT,"s");
                        _uiAdd(deviceTag(),H4P_UI_TEXT,"s");
                        _uiAdd(boardTag(),H4P_UI_TEXT,"s");
                        _uiAdd(NBootsTag(),H4P_UI_TEXT,"s");
                        _uiAdd(ipTag(),H4P_UI_TEXT,"s"); // cos we don't know it yet
                    }
                #else
                    _clearUI();
                    _uiAdd(h4pTag(),H4P_UI_TEXT,"s",H4P_VERSION);
                    _uiAdd(chipTag(),H4P_UI_TEXT,"s");
                    _uiAdd(deviceTag(),H4P_UI_TEXT,"s");
                    _uiAdd(boardTag(),H4P_UI_TEXT,"s");
                    _uiAdd(NBootsTag(),H4P_UI_TEXT,"s");
                    _uiAdd(ipTag(),H4P_UI_TEXT,"s"); // cos we don't know it yet
                #endif
                    h4psysevent("viewers",H4PE_VIEWERS,"%d",WiFi.getMode());
                    h4pUIorder.shrink_to_fit();
                }
                for(auto const& ui:h4pUIorder){
                    auto i=h4pUserItems[ui];
                    _sendSSE("ui",CSTR(std::string(ui+","+stringFromInt(i.type)+","+i.h+",0,"+stringFromInt(i.color)+","+i.f())));
                }
                h4.repeatWhile([this]{ return _evts->count(); },
                    ((H4WF_EVT_TIMEOUT*3)/4),
                    []{},
                    []{ h4psysevent("viewers",H4PE_VIEWERS,"%d",0); },
                    H4P_TRID_SSET,true
                );
            });
        }
    });
    addHandler(_evts);

    on("/",HTTP_GET, [this](AsyncWebServerRequest *request){
        XLOG("FH=%u --> Root %s",_HAL_freeHeap(),request->client()->remoteIP().toString().c_str());
        request->send(HAL_FS,"/sta.htm",String(),false,_aswsReplace);
    });

	on("/rest",HTTP_GET,[this](AsyncWebServerRequest *request){ _rest(request); });

    serveStatic("/", HAL_FS, "/").setCacheControl("max-age=31536000");
    begin();
}

void H4P_WiFi::_stopWebserver(){ 
#if defined ARDUINO_ARCH_ESP32
    ArduinoOTA.end(); // WHYYYYYYYYYY???? FFS
    MDNS.end();
#endif
    end();
    _discoDone=true;
    _clearUI();
    svcDown();
}

void H4P_WiFi::_uiAdd(const std::string& name,H4P_UI_TYPE t,const std::string& h,const std::string& value,uint8_t color){
    std::function<std::string(void)>  f;
    std::string v=value;
    switch(t){
        case H4P_UI_GPIO:
            f=[=](){ return stringFromInt(H4P_PinMachine::logicalRead(STOI(name))); };
            break;
        case H4P_UI_DROPDOWN:
            if(!h4p.gvExists(name)) h4p.gvSetstring(name,"");
            f=[=]{ return value; };
            break;
        default:
            if(v.size()) f=[=](){ return v; };
            else {
                if(!h4p.gvExists(name)) h4p.gvSetstring(name,value,false);
                f=[=](){ return h4p[name]; };
            }
    }
    h4pUIorder.push_back(name);
    h4pUserItems[name]={t,f,color,h};
}
/*

        PUBLICS

*/
void H4P_WiFi::change(std::string ssid,std::string psk){ // add device / name?
    h4p[pskTag()]=psk;
    h4p[ssidTag()]=ssid;
    _restart();
#if H4P_WIFI_USE_AP
    if(_dns53) QEVENT(H4PE_REBOOT);
#endif
}

#if H4P_LOG_MESSAGES
void H4P_WiFi::info() { 
    H4Service::info();
    reply("Radio is %s Device %s Mode=%d Status: %d IP=%s",WiFi.getMode()==WIFI_OFF ? "OFF":"ON",CSTR(h4p[deviceTag()]),WiFi.getMode(),WiFi.status(),WiFi.localIP().toString().c_str());
    reply(" SSID %s PSK=%s",CSTR(WiFi.SSID()),CSTR(WiFi.psk()));
    #ifndef H4P_ASSUMED_LED
        reply(" ** NO Signal Pin! **");
    #endif
}
#endif

void H4P_WiFi::svcDown(){
    _signalBad();
    h4.cancelSingleton(H4P_TRID_HOTA);
    H4Service::svcDown();
}

void H4P_WiFi::uiAddAllUsrFields(const std::string& section){ for(auto const& g:h4pGlobal) if(g.first.rfind("usr_")!=std::string::npos) _uiAdd(replaceAll(g.first,"usr_",""),H4P_UI_TEXT,section,g.second); }

void H4P_WiFi::uiAddDropdown(const std::string& name,H4P_NVP_MAP options,const std::string& section){ _uiAdd(name,H4P_UI_DROPDOWN,section,flattenMap(options)); }