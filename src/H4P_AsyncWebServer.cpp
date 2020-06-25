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
#include<H4P_WiFiSelect.h>
#ifndef H4P_NO_WIFI
#include<H4P_AsyncWebServer.h>
#include<H4P_CmdErrors.h>
#include<H4P_SerialCmd.h>

void  H4P_AsyncWebServer::_hookIn(){ 
    DEPEND(wifi);
    H4Plugin* p=isLoaded(onofTag());
    if(p) _btp=reinterpret_cast<H4P_BinaryThing*>(p);
}

uint32_t H4P_AsyncWebServer::_msg(vector<string> vs){
    return guard1(vs,[this](vector<string> vs){
        sendUIMessage(H4PAYLOAD);
        return H4_CMD_OK;
    });
}
uint32_t H4P_AsyncWebServer::_uib(vector<string> vs){
    return guardString2(vs,[this](string a,string b){ 
        if(isNumeric(b)) {
            uint32_t bv=atoi(CSTR(b));
            if(bv < 2) {
                if(userItems.count(a)){
                    _sendSSE(CSTR(a),CSTR(b));
                    userItems[a].a(a,b);
                    return H4_CMD_OK;
                } else return H4_CMD_NAME_UNKNOWN;
            } else return H4_CMD_PAYLOAD_FORMAT;
        } else return H4_CMD_NOT_NUMERIC;
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
            for(auto &l:lines){
                if(l.back()=='\n') l.pop_back();
                fl+="\""+l+"\",";
            }
            if(fl.size()) fl.pop_back();
        }
        j+=fl+"]}";
        request->send(200,"application/json",CSTR(j));
        lines.clear();
	},request),nullptr,H4P_TRID_ASWS);
}

void  H4P_AsyncWebServer::_sendSSE(const char* name,const char* msg){ if(_evts && _evts->count()) _evts->send(msg,name,_evtID); }

void H4P_AsyncWebServer::_setBothNames(const string& host,const string& friendly){
    if(isLoaded(upnpTag())) h4wifi._setPersistentValue(nameTag(),friendly,false);
    h4wifi.host(host);
}

void H4P_AsyncWebServer::_start(){
	reset();
    _evts=new AsyncEventSource("/evt");
    _evts->onConnect([this](AsyncEventSourceClient *client){
        //Serial.printf("T=%u SSE CLIENT\n",millis());
//        client->send("HELLO",NULL,++_evtID,H4P_ASWS_EVT_TIMEOUT);
        h4.queueFunction([this,client](){
//            H4EVENT("SSE Client %08x n=%d T/O=%d nC=%d nUI=%d",client,client->lastId(),H4P_ASWS_EVT_TIMEOUT,_evts->count(),userItems.size());
            if(_evts->count()==1) if(_onC) _onC(); // first time only
            for(auto const& i:userItems) {
//                H4EVENT("build UI %s\n",CSTR(string(i.first+","+stringFromInt(i.second.type)+","+i.second.f()+","+(i.second.a ? "1":"0" )))); 
                _sendSSE("ui",CSTR(string(i.first+","+stringFromInt(i.second.type)+","+i.second.f()+","+(i.second.a ? "1":"0" ))));
            }
            h4.repeatWhile([this]{ return _evts->count(); },
                ((H4P_ASWS_EVT_TIMEOUT*3)/4),
                [this]{ _sendSSE("","ka"); },
                [this]{ if(_onD) _onD(); },
                H4P_TRID_SSET,true
            );
        });
    });
    addHandler(_evts);

    on("/",HTTP_GET, [this](AsyncWebServerRequest *request){
        H4EVENT("Root %s",request->client()->remoteIP().toString().c_str());
        _cb[wifiTag()]=stringFromInt(WiFi.getMode());
        request->send(SPIFFS,"/sta.htm",String(),false,aswsReplace);
    });

    on("/",HTTP_POST, [this](AsyncWebServerRequest *request){
        H4EVENT("Root POST %s",request->client()->remoteIP().toString().c_str());
    	H4P_CONFIG_BLOCK rp;
	    int params = request->params();
	    for(int i=0;i<params;i++){
		    AsyncWebParameter* p = request->getParam(i);
		    rp[CSTR(p->name())]=CSTR(p->value());
	    }
        h4wifi.change(rp[ssidTag()],rp[pskTag()]);
        _setBothNames(rp[deviceTag()],rp[nameTag()]);
    });

	on("/rest",HTTP_GET,[this](AsyncWebServerRequest *request){ _rest(request); });

    serveStatic("/", SPIFFS, "/").setCacheControl("max-age=31536000");
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

void H4P_AsyncWebServer::sendUIMessage(const string& msg,...){
    char buff[H4P_REPLY_BUFFER+1];
    va_list ap; 
    va_start(ap, msg); 
    vsnprintf(buff,H4P_REPLY_BUFFER,CSTR(msg),ap);
    va_end(ap);
    _sendSSE(NULL,buff);
}

#endif // H4_WIFI