
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
#include<H4P_BinaryThing.h>

void __attribute__((weak)) onRTC(){}

H4P_Timekeeper::H4P_Timekeeper(const string& ntp1,const string& ntp2,int tzo,H4_FN_VOID onC,H4_FN_VOID onD): _ntp1(ntp1),_ntp2(ntp2),_tzo(tzo),H4Plugin(timeTag(),onC,onD){
    _cmds={
        {_pName,   { H4PC_H4, _subCmd, nullptr}}, // root for this plugin, e.g. h4/ME...
        {"change", { _subCmd,       0, CMDVS(_change)}},
        {"sync",   { _subCmd,       0, CMD(sync)}},
        {"tz",     { _subCmd,       0, CMDVS(_tz)}}
    };
}

uint32_t H4P_Timekeeper::__alarmCore (vector<string> vs,bool daily,H4BS_FN_SWITCH f){
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
    if(_mss00){ // onRTC
        H4EVENT("%s %s -> %s",daily ? "Daily":"S/Shot",CSTR((a)),CSTR(b)? "ON":"OFF");
        uint32_t msDue=_msDue(CSTR((a)));
        int onoff=atoi(CSTR(b));
        //pq::add(H4_FN_VOID, uint32_t, uint32_t, H4_FN_COUNT, H4_FN_VOID, uint32_t, bool)
        uint32_t u=daily ? H4P_TRID_DALY:H4P_TRID_SHOT;
        h4.add([this,f,onoff]{ 
            if(_mss00) {
                Serial.printf("ALARM %s => %d\n",CSTR(clockTime()),onoff);
                f(onoff);
            }
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

uint32_t H4P_Timekeeper::_at(vector<string> vs){ 
 //   return __alarmCore(vs,false,[this](bool b){ _btp->switch(b); }); 
}
uint32_t H4P_Timekeeper::_daily(vector<string> vs){ 
//    return __alarmCore(vs,true,[this](bool b){ _btp->switch(b); }); 
}

uint32_t H4P_Timekeeper::_change(vector<string> vs){ return guardString2(vs,[this](string a,string b){ change(a,b); }); }

void H4P_Timekeeper::_hookIn(){ 
    DEPEND(wifi);
    H4Plugin* p=isLoaded(onofTag());
    if(p) {
        _btp=reinterpret_cast<H4P_BinaryThing*>(p);
        h4cmd.addCmd("at",_subCmd,0,CMDVS(_at));
        h4cmd.addCmd("daily",_subCmd,0,CMDVS(_daily));
    }
}

uint32_t H4P_Timekeeper::_msDue(const string& rtc){
    int msDue=parseTime(rtc)-msSinceMidnight(); // upcast
    if(msDue < 0) msDue+=24*60*60*1000;
    return static_cast<uint32_t>(msDue);
}

void H4P_Timekeeper::_start(){
//    Serial.print(CSTR(_pName));Serial.println(" _start");
//    for some reason it never seems to sync first time
    _useNTP();
}

void H4P_Timekeeper::_stop(){ 
    h4.cancelSingleton({H4P_TRID_TIME,H4P_TRID_SYNC});
	sntp_stop();
 }

 uint32_t H4P_Timekeeper::_tz(vector<string> vs){
    return guardInt1(vs,[this](int v){
        tz(v); 
        return H4_CMD_OK;
    });
}

void H4P_Timekeeper::_useNTP(){

	sntp_set_timezone(_tzo); 
	ip_addr_t *s1=__ntpSetServer(0,CSTR(_ntp1));
	ip_addr_t *s2=__ntpSetServer(1,CSTR(_ntp2));
	if(s1) os_free(s1);
	if(s2) os_free(s2);

	sntp_init();
    
    h4.repeatWhile([this]{ return !_mss00; },
        H4P_TIME_HOLDOFF,
        [this]{ sync(); },
        [this]{
            static bool gotRTC=false; 
            H4EVENT("%u NTP %s",millis(),CSTR(clockTime()));
            if(!gotRTC) {
                onRTC();
                gotRTC=true;
            }
        },
        H4P_TRID_TIME,true
    );
    h4.every(H4P_TIME_RESYNC,[this]{ sync(); },nullptr,H4P_TRID_SYNC,true); // tweakables
}
//
//
//
void H4P_Timekeeper::at(const string& when,bool onoff,H4BS_FN_SWITCH f){
    if(_btp) __alarmCore({when,stringFromInt(onoff)},false,f);
}

void H4P_Timekeeper::change(const string& ntp1,const string& ntp2){
    _stop();
    _ntp1=ntp1;
    _ntp2=ntp2;
    _start();
}

void H4P_Timekeeper::daily(const string& when,bool onoff,H4BS_FN_SWITCH f){
    if(_btp) __alarmCore({when,stringFromInt(onoff)},true,f);
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

const char* H4P_Timekeeper::secondsFromNow(uint32_t s){
    static char buf[]="15:15";
    //CSTR(h4tk.strTime(h4tk.parseTime(h4tk.msSinceMidnight()+30)))
    return buf;  
}

void H4P_Timekeeper::show(){ 
    reply("TZO=%d",_tzo);
    reply("ntp1=%s",CSTR(_ntp1));
    reply("ntp2=%s",CSTR(_ntp2));
    reply("UpTime=%s",CSTR(upTime()));
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

void H4P_Timekeeper::tz(uint32_t tzOffset){
    _stop();
    _tzo=tzOffset;
    _start();
}

string H4P_Timekeeper::upTime(){
	uint32_t t=millis()/1000;
	return stringFromInt(t / 86400,"%02d:")+strTime(t);
}

#endif // esp8266 only