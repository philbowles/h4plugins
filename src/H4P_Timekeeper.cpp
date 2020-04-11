
/*
 MIT License

Copyright (c) 2019 Phil Bowles <h4plugins@gmail.com>
   github     https://github.com/philbowles/H4P_Timekeeper
   blog       https://8266iot.blogspot.com     
   groups     https://www.facebook.com/groups/esp8266questions/
              https://www.facebook.com/H4P_Timekeeper-Esp8266-Firmware-Support-2338535503093896/
                			  

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
#ifdef ARDUINO_ARCH_ESP8266
#include<H4P_Timekeeper.h>
#include<H4P_SerialCmd.h>

H4P_Timekeeper::H4P_Timekeeper(const char* ntp1,const char* ntp2,int tzo,H4_FN_VOID onC,H4_FN_VOID onD): _ntp1(ntp1),_ntp2(ntp2),_tzo(tzo),H4Plugin(timeTag(),onC,onD){
    _cmds={
        {_pName,   { H4PC_H4, _subCmd, nullptr}}, // root for this plugin, e.g. h4/ME...
        {"at",     { _subCmd,       0, CMDVS(_at)}}, // dyn add
        {"daily",  { _subCmd,       0, CMDVS(_daily)}}, // dyn add
        {"sync",   { _subCmd,       0, CMD(sync)}},
        {"tz",     { _subCmd,       0, CMDVS(_tz)}}
    };
    _useNTP(tzo,ntp1,ntp2); 
/*    
    daily("00:00",[](){
        for(auto s:_statistics) s.reset(); // midnight chain to georgia
        sync();
        },nullptr,nullptr,0,true
    );
*/
}

uint32_t H4P_Timekeeper::__alarmCore (vector<string> vs,bool daily){
//    TIDY!! and invert! 
    vector<string> vg=split(H4PAYLOAD,",");
    if(vg.size()>2) return H4_CMD_TOO_MANY_PARAMS;
    if(vg.size()<2) return H4_CMD_TOO_FEW_PARAMS;
    string a=vg[0];
    string b=vg[1];
    if(!isNumeric(b)) return H4_CMD_NOT_NUMERIC;
    vector<string> hhmm=split(a,":");
    if(hhmm.size()!=2) return H4_CMD_PAYLOAD_FORMAT;
    int hh=atoi(CSTR(hhmm[0]));
    int mm=atoi(CSTR(hhmm[1]));
    if(!(hh<24 && mm<60)) return H4_CMD_PAYLOAD_FORMAT;
    if(_mss00){
        H4EVENT("%s %s -> %s",daily ? "Daily":"S/Shot",CSTR((a)),CSTR(b)? "ON":"OFF");
        uint32_t msDue=_msDue(CSTR((a)));
        int onoff=atoi(CSTR(b));
        //pq::add(H4_FN_VOID, uint32_t, uint32_t, H4_FN_COUNT, H4_FN_VOID, uint32_t, bool)
        uint32_t u=daily ? H4P_TRID_DALY:H4P_TRID_SHOT;
        h4.add([this,onoff]{ 
            if(_mss00) Serial.printf("ALARM %s => %d\n",CSTR(clockTime()),onoff);
            else H4EVENT("RTC ignored"); // don't fire if NTP not sync'ed 
        },msDue,daily ? msDue:0,H4Countdown(1),nullptr,TAG(daily ? 7:3));
        return H4_CMD_OK;
    } else H4EVENT("Alarm ignored"); // don't set if NTP not sync'ed 
}

ip_addr_t * H4P_Timekeeper::__ntpSetServer(int n,const char* ntp){
	if(atoi(ntp)) {
		ip_addr_t *addr = (ip_addr_t *)os_zalloc(sizeof(ip_addr_t));
		ipaddr_aton(ntp, addr);
		sntp_setserver(n, addr); // set server 2 by IP address
		return addr;
	} else sntp_setservername(n,const_cast<char*>(ntp));
	return nullptr;
}

void H4P_Timekeeper::_hookIn(){ DEPEND(wifi); }

uint32_t H4P_Timekeeper::_msDue(const char* rtc){
    int msDue=parseTime(rtc)-msSinceMidnight(); // upcast
    if(msDue < 0) msDue+=24*60*60*1000;
    return static_cast<uint32_t>(msDue);
}

void H4P_Timekeeper::_start(){
//    Serial.print(CSTR(_pName));Serial.println(" _start");
//    for some reason it never seems to sync first time
    h4.repeatWhile([this]{ return !_mss00; },
        H4P_TIME_HOLDOFF,
        [this]{ sync(); },
        [this]{ H4EVENT("%u NTP %s",millis(),CSTR(clockTime())); },
        H4P_TRID_TIME,true
    );
    h4.every(H4P_TIME_RESYNC,[this]{ sync(); },nullptr,H4P_TRID_SYNC,true); // tweakables
}

void H4P_Timekeeper::_stop(){ h4.cancelSingleton({H4P_TRID_TIME,H4P_TRID_SYNC}); }

void H4P_Timekeeper::_useNTP(int offset,const char* srv1,const char* srv2){
	sntp_stop();
	sntp_set_timezone(offset); 
	ip_addr_t *s1=__ntpSetServer(0,srv1);
	ip_addr_t *s2=__ntpSetServer(1,srv2);
	sntp_init();
	if(s1) os_free(s1);
	if(s2) os_free(s2);
}

H4_TIMER H4P_Timekeeper::at(const char* when,H4_FN_VOID f,H4_FN_VOID fnc,uint32_t id){

}

H4_TIMER H4P_Timekeeper::daily(const char* when,H4_FN_VOID f,H4_FN_VOID fnc,uint32_t id){

}

uint32_t H4P_Timekeeper::parseTime(const string& ts){
	//string t(ts);
	uint32_t  h=0;
	uint32_t  m=0;
	uint32_t  s=0;
	vector<string> parts=split(ts,":");
	if(parts.size() < 4){
	if(parts.size() > 2) s=atoi(CSTR(parts[2]));
		m=atoi(CSTR(parts[1]));
		h=atoi(CSTR(parts[0]));  
		}
	return 1000*(s+(m*60) + (h*3600));
}

string H4P_Timekeeper::strTime(uint32_t t){
	char buf[9];
	sprintf(buf,"%02d:%02d:%02d",(t%86400)/3600,(t/60)%60,t%60);
	return string(buf);
}

void H4P_Timekeeper::sync(){
	long stamp=sntp_get_current_timestamp();
    Serial.printf("Raw stamp %lu\n",stamp);
	if(stamp > 30000){ // 28800 +leeway: default is GMT+8
		H4EVENT("%u time %lu",millis(),stamp);
		vector<string> dp=split(sntp_get_real_time(stamp)," ");
		string year=dp.back();
        dp.pop_back();
		string time=dp.back();
        dp.pop_back();
		dp.push_back(year);
		string date=join(dp," ");
		date.pop_back();
		_cb["date"]=date; // why?
        _mss00=parseTime(time);
        H4EVENT("%s %s _mss00=%d",CSTR(date),CSTR(time),_mss00);
//		onTimeSync(stamp);
	} else H4EVENT("%u NO TIME AT ALL",millis());
}

string H4P_Timekeeper::upTime(){
	uint32_t t=millis()/1000;
	return stringFromInt(t / 86400,"%02d:")+strTime(t);
}

#endif // esp8266 only