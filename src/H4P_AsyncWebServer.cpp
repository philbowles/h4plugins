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
#include<H4P_AsyncWebServer.h>
#include<H4P_CmdErrors.h>
#include<H4P_SerialCmd.h>
#include<H4P_GPIOManager.h>

void __attribute__((weak)) onUiChange(const string& name,const string& value){ h4UserEvent("onUiChange %s now=%s",CSTR(name),CSTR(value)); }

void  H4P_AsyncWebServer::_hookIn(){ 
    DEPEND(wifi);
    H4Plugin* p=isLoaded(onofTag());
    if(p) _btp=reinterpret_cast<H4P_BinaryThing*>(p);

    _uiAdd(3,boardTag(),H4P_UI_LABEL);
    _uiAdd(4,chipTag(),H4P_UI_LABEL);
    if(WiFi.getMode()==WIFI_STA){
        _uiAdd(1,deviceTag(),H4P_UI_LABEL);
        _uiAdd(5,ipTag(),H4P_UI_LABEL,"",[]{ return _cb[ipTag()]; }); // cos we don't know it yet
        _uiAdd(6,"H4v",H4P_UI_LABEL,H4_VERSION);
        _uiAdd(7,h4PvTag(),H4P_UI_LABEL);
        _uiAdd(8,h4UIvTag(),H4P_UI_LABEL);
        if(isLoaded(mqttTag())) _uiAdd(8,"Pangolin Vn",H4P_UI_LABEL,_cb[pmvTag()]);
    }
    else {
//        _uiAdd(1,"Device",H4P_UI_INPUT,_cb[deviceTag()],nullptr,onUiChange); 
        WiFi.enableSTA(true);
        int n=WiFi.scanNetworks();
        H4P_CONFIG_BLOCK ssids;
        for (uint8_t i = 0; i < n; i++) {
            string ss=CSTR(WiFi.SSID(i));
            ssids[CSTR(WiFi.SSID(i))]=ss;
        }
/*
        h4asws.uiAddInput(deviceTag());
        if(isLoaded(nameTag())) h4asws.uiAddInput(nameTag());
        h4asws.uiAddDropdown(ssidTag(),ssids);
        h4asws.uiAddInput(pskTag());
        h4asws._uiAdd("Connect to SSID -> ",H4P_UI_BOOL,"",
            nullptr,
            [this](const string& a,const string& b){ 
                H4EVENT("User config connect to %s/%s",CSTR(_cb[ssidTag()]),CSTR(_cb[pskTag()]));
                H4EVENT("device / friendly %s/%s",CSTR(_cb[deviceTag()]),CSTR(_cb[nameTag()]));
                //h4wifi.setBothNames(_cb[deviceTag()],_cb[nameTag()]);
                //h4wifi.change(_cb[ssidTag()],_cb[pskTag()]);
           });
*/
        WiFi.scanDelete();
        WiFi.enableSTA(false); // force AP only
    }
}

uint32_t H4P_AsyncWebServer::_gpio(vector<string> vs){
    return guard1(vs,[this](vector<string> vs){
        if(H4PAYLOAD=="all") uiAddGPIO();
        else {
            if(!isNumeric(H4PAYLOAD)) return H4_CMD_NOT_NUMERIC;
            uint8_t pin=H4PAYLOAD_INT;
            Serial.printf("uiAdd GPIO pin %d\n",pin);
            return uiAddGPIO(pin);
        }
        return H4_CMD_OK;
    });
}

uint32_t H4P_AsyncWebServer::_msg(vector<string> vs){
    return guard1(vs,[this](vector<string> vs){
        uiMessage(H4PAYLOAD);
        return H4_CMD_OK;
    });
}

H4_CMD_ERROR H4P_AsyncWebServer::__uichgCore(const string& a,const string& b,H4P_FN_UICHANGE f){
    _sendSSE(CSTR(a),CSTR(b));
    f(b);
    return H4_CMD_OK;
}

void H4P_AsyncWebServer::_uiAdd(int seq,const string& i,H4P_UI_TYPE t,const string& v,H4P_FN_UITXT f,H4P_FN_UICHANGE c){
    string value=v;
    if(c) value=_cb[i]; // change function implies input / dropdown etc: backed by CB variable
    else {
        if(value=="") { // when no intial value given...
            if(f) value=f(); // if dynamic, sync it, or...
            else value=_cb[i]; // assume its backed by CB variable
        }
    }
    _userItems[seq]={i,t,value,f,c};
}

uint32_t H4P_AsyncWebServer::_uichg(vector<string> vs){
    return guardString2(vs,[this](string a,string b){ 
        for(auto &i:_userItems){
            H4P_UI_ITEM u=i.second;
            if(u.id==a){
                if(u.c){
                    if(u.type==H4P_UI_BOOL){
                        if(isNumeric(b)) {
                            if(atoi(CSTR(b)) > 1) return H4_CMD_PAYLOAD_FORMAT;
                        } else return H4_CMD_NOT_NUMERIC;
                    }
                    else _cb[a]=b;
                    return __uichgCore(a,b,u.c);
                }
                return H4_CMD_OUT_OF_BOUNDS; // not updateable
            }
        }
        return H4_CMD_NAME_UNKNOWN;
    });
}

void H4P_AsyncWebServer::_rest(AsyncWebServerRequest *request){
	h4.queueFunction(bind([this](AsyncWebServerRequest *request){
        H4EVENT("_rest %s",request->client()->remoteIP().toString().c_str());
		string chop=replaceAll(CSTR(request->url()),"/rest/","");
        string msg="";
        uint32_t res=h4cmd._simulatePayload(CSTR(chop),aswsTag());
        if(isLoaded(cerrTag())) msg=h4ce.getErrorMessage(res);
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
	},request),nullptr,H4P_TRID_ASWS);
}

void H4P_AsyncWebServer::_sendSSE(const char* name,const char* msg){ if(_evts && _evts->count()) _evts->send(msg,name,_evtID); }

void H4P_AsyncWebServer::_start(){
	reset();
    _evts=new AsyncEventSource("/evt");
    _evts->onConnect([this](AsyncEventSourceClient *client){
        h4.queueFunction([this,client](){
            H4EVENT("SSE Client %08x n=%d T/O=%d nC=%d nUI=%d",client,client->lastId(),H4P_ASWS_EVT_TIMEOUT,_evts->count(),_userItems.size());
            if(_evts->count()==1) if(_onC) _onC(); // first time only R WE SURE?
            for(auto & i:_userItems) {
                H4P_UI_ITEM u=i.second;
                if(u.f) u.value=u.f(); // auto-sync delayed fields
                _sendSSE("ui",CSTR(string(u.id+","+stringFromInt(u.type)+","+u.value+","+(u.c ? "1":"0" ))));
            }
            h4.repeatWhile([this]{ return _evts->count(); },
                ((H4P_ASWS_EVT_TIMEOUT*3)/4),
                [this]{ /* _sendSSE("","ka"); */ },
                [this]{ if(_onD) _onD(); }, // ???
                H4P_TRID_SSET,true
            );
        });
    });
    addHandler(_evts);

    on("/",HTTP_GET, [this](AsyncWebServerRequest *request){
        H4EVENT("Root %s",request->client()->remoteIP().toString().c_str());
        _cb[wifiTag()]=stringFromInt(WiFi.getMode());
        request->send(HAL_FS,"/sta.htm",String(),false,aswsReplace);
    });
/*
    on("/",HTTP_POST, [this](AsyncWebServerRequest *request){
        H4EVENT("Root POST %s",request->client()->remoteIP().toString().c_str());
    	H4P_CONFIG_BLOCK rp;
	    int params = request->params();
	    for(int i=0;i<params;i++){
		    AsyncWebParameter* p = request->getParam(i);
		    rp[CSTR(p->name())]=CSTR(p->value());
	    }
        h4wifi.setBothNames(rp[deviceTag()],rp[nameTag()]);
        h4wifi.change(rp[ssidTag()],rp[pskTag()]);
    });
*/
	on("/rest",HTTP_GET,[this](AsyncWebServerRequest *request){ _rest(request); });

    serveStatic("/", HAL_FS, "/").setCacheControl("max-age=31536000");
    // 404 ?
    begin();
    _upHooks();
}

void H4P_AsyncWebServer::_stop(){ 
    end();
    _downHooks();
}

String H4P_AsyncWebServer::aswsReplace(const String& var){
    string v=CSTR(var);
    return _cb.count(v) ? String(CSTR(_cb[v])):"?";
}

void H4P_AsyncWebServer::show(){ 
    for(auto &i:_userItems) {
        H4P_UI_ITEM u=i.second;
        reply("id=%d item=%s value=%s f=%08x c=%08x",i.first,CSTR(u.id),CSTR(u.value),&u.f,&u.c);
    }
}
/*
void H4P_AsyncWebServer::uiAddDropdown(const string& name,H4P_CONFIG_BLOCK options){
    string opts="";
    for(auto const& o:options) opts+=o.first+"="+o.second+",";
    opts.pop_back();
//    _uiAdd(name,H4P_UI_DROPDOWN,"",[opts]{ return opts; },onUiChange);
}
*/
void H4P_AsyncWebServer::uiAddGPIO(){ if(isLoaded(gpioTag())) for(auto const& p:H4P_GPIOManager::pins) uiAddGPIO(p.first); }

H4_CMD_ERROR H4P_AsyncWebServer::uiAddGPIO(uint8_t pin){
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
            _uiAdd(300+pin,name,H4P_UI_BOOL,"",[p]{ return p->logicalRead() ? "1":"0"; });
            return H4_CMD_OK;
        } 
        return H4_CMD_OUT_OF_BOUNDS;
    }
    return H4_CMD_NOT_NOW;
}
/*
void H4P_AsyncWebServer::uiAddInput(const string& name,H4P_FN_UITXT f){
    if(f) _cb[name]=f();
    _uiAdd(name,H4P_UI_INPUT,"",[name]{ return _cb[name]; },onUiChange); 
}
void H4P_AsyncWebServer::uiAddInput(const string& name,const string& value){
    _uiAdd(name,H4P_UI_INPUT,_cb[name],nullptr,onUiChange); 
}
*/
void H4P_AsyncWebServer::uiSync(){
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