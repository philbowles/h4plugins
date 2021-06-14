
/*
 MIT License

Copyright (c) 2020 Phil Bowles <h4plugins@gmail.com>
   github     https://github.com/philbowles/h4plugins
   blog       https://8266iot.blogspot.com     
   groups     https://www.facebook.com/groups/esp8266questions/
              https://www.facebook.com/H4P_Timekeeper-Esp8266-Firmware-Support-2338535503093896/
                			  
Portions (c) 2020 Adam Sharp http://www.threeorbs.co.uk/

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
#include<H4P_Timekeeper.h>
#include<H4P_EmitTick.h>
#include<H4P_WiFi.h>

constexpr uint32_t secsInDay(){ return 86400; }
constexpr uint32_t msInDay(){ return 1000*secsInDay(); }

H4P_Timekeeper::H4P_Timekeeper(const std::string& ntp1,const std::string& ntp2,int tzo,H4_FN_DST fDST): _fDST(fDST), H4Service(timeTag(),H4PE_HEARTBEAT){
    depend<H4P_EmitTick>(tickTag());
    depend<H4P_WiFi>(wifiTag());
    _addLocals({
        {_me,      { H4PC_H4, _pid, nullptr}}, // root for this plugin
        {"at",     { _pid,       0, CMDVS(_at)}},
        {"change", { _pid,       0, CMDVS(_change)}},
        {"daily",  { _pid,       0, CMDVS(_daily)}},
        {"sync",   { _pid,       0, CMD(sync)}},
        {"tz",     { _pid,       0, CMDVS(_tz)}}
    });
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    _setupSNTP(ntp1,ntp2);
    __HALsetTimezone(0);
    _tzo = tzo;
    if(!_fDST) _fDST=[](uint32_t){ return 0; };
}

#ifdef ARDUINO_ARCH_ESP8266
void H4P_Timekeeper::__HALsetTimezone(int tzo){
    _tzo=tzo;
    sntp_set_timezone(0);
}

void H4P_Timekeeper::sync(){
	long stamp=sntp_get_current_timestamp();
    XLOG("TK sync 8266 stamp=%lu",stamp);
	if(stamp > 30000){ // 28800 +leeway: default is GMT+8
        stamp+=(_tzo*60);
		std::vector<std::string> dp=split(replaceAll(sntp_get_real_time(stamp + _fDST(stamp)),"  "," ")," ");
        _mss00=parseTime(dp[3])-millis();
	}
}
#else 
void H4P_Timekeeper::__HALsetTimezone(int tzo){
    _tzo=tzo;
    std::string tz="GMT"+stringFromInt((-1*tzo),"%+02d");
    setenv("TZ", CSTR(tz), 1);
    tzset();
 }

void H4P_Timekeeper::sync(){
    time_t now = 0;
    time(&now);
    uint32_t adjusted=now+_tzo*60;
    now=adjusted + _fDST(adjusted);
    localtime_r(&now, &_timeinfo);
    if(_timeinfo.tm_year > 119) _mss00=(1000*((3600*_timeinfo.tm_hour)+(60*_timeinfo.tm_min)+_timeinfo.tm_sec))-millis();
}
#endif

uint32_t H4P_Timekeeper::__alarmCore (std::vector<std::string> vs,bool daily){
    std::vector<std::string> vg=split(H4PAYLOAD,",");
    if(vg.size()>2) return H4_CMD_TOO_MANY_PARAMS;
    if(vg.size()<2) return H4_CMD_TOO_FEW_PARAMS;
    int T=parseTime(vg[0]);
    if(T<0) return H4_CMD_PAYLOAD_FORMAT;
    std::string b=vg[1];
    if(!stringIsNumeric(b)) return H4_CMD_NOT_NUMERIC;
    int onoff=atoi(CSTR(b));
    if(_mss00){ // onRTC
        XLOG("%s %s -> %s",daily ? "Daily":"S/Shot",CSTR(strTime(T)),onoff ? "ON":"OFF");
        int msDue=T-msSinceMidnight();
        if(msDue < 0) msDue+=msInDay();
        uint32_t u=daily ? H4P_TRID_DALY:H4P_TRID_SHOT;
        h4.add([=]{ if(_mss00) XEVENT(H4PE_ALARM,"%d",onoff); },msDue,daily ? msDue:0,H4Countdown(1),nullptr,TAG(daily ? 7:3));
        return H4_CMD_OK;
    }
    return H4_CMD_NOT_NOW; 
}

uint32_t H4P_Timekeeper::_at(std::vector<std::string> vs){ return __alarmCore(vs,false); }

uint32_t H4P_Timekeeper::_change(std::vector<std::string> vs){ return _guardString2(vs,[=](std::string a,std::string b){ change(a,b); return H4_CMD_OK; }); }

uint32_t H4P_Timekeeper::_daily(std::vector<std::string> vs){ return __alarmCore(vs,true); }

void H4P_Timekeeper::_handleEvent(const std::string& svc,H4PE_TYPE t,const std::string& msg){
    if(t==H4PE_HEARTBEAT){
        h4p[timeTag()]=clockTime();
        h4p[upTimeTag()]=upTime();
    }
}

void H4P_Timekeeper::_init(){
    if(WiFi.getMode()==WIFI_STA){
        h4puiAdd("NTP1",H4P_UI_TEXT,"t",_ntp1); // cos we don't know it yet
        h4puiAdd("NTP2",H4P_UI_TEXT,"t",_ntp2);
        h4puiAdd("TZ",H4P_UI_TEXT,"t",stringFromInt(_tzo));
        h4puiAdd(timeTag(),H4P_UI_TEXT,"t"); // cos we don't know it yet
        h4puiAdd(upTimeTag(),H4P_UI_TEXT,"t");
    }
}

void H4P_Timekeeper::_setupSNTP(const std::string& ntp1, const std::string& ntp2){
    sntp_stop();
    _ntp1=ntp1;
    _ntp2=ntp2;
    sntp_setservername(0,(char*) _ntp1.c_str());
    sntp_setservername(1,(char*) _ntp2.c_str());
}

uint32_t H4P_Timekeeper::_tz(std::vector<std::string> vs){
    return _guardInt1(vs,[this](int v){
        tz(v); 
        return H4_CMD_OK;
    });
}
//
//
//
void H4P_Timekeeper::at(const std::string& when,bool onoff){ __alarmCore({when+","+stringFromInt(onoff)},false); }

void H4P_Timekeeper::change(const std::string& ntp1,const std::string& ntp2){ _setupSNTP(ntp1,ntp2); }

void H4P_Timekeeper::daily(const std::string& when,bool onoff){ __alarmCore({when+","+stringFromInt(onoff)},true); }

#if H4P_LOG_MESSAGES
void H4P_Timekeeper::info(){
    H4Service::info();
    reply(" _mss00=%d, ms since 00:00=%d",_mss00,msSinceMidnight());
    reply(" TZO=%d",_tzo);
    reply(" Servers: %s %s",CSTR(_ntp1),CSTR(_ntp2));
    reply(" Wallclock=%s, UpTime=%s",CSTR(clockTime()),CSTR(upTime()));
}
#endif

std::string H4P_Timekeeper::minutesFromNow(uint32_t m){
    std::string longtime=strTime((msSinceMidnight())+(m*60000));  
    return std::string(longtime.begin(),longtime.end()-3); 
}

int H4P_Timekeeper::parseTime(const std::string& ts){ // in milliseconds!
	std::vector<std::string> parts=split(ts,":");
    uint32_t    h,m,s=0;
    switch(parts.size()){
        case 3:
	        s=atoi(CSTR(parts[2]));
            if(s>59) return -1;
        case 2:
            m=atoi(CSTR(parts[1]));
            if(m>59) return -1;
            h=atoi(CSTR(parts[0]));
            if(h>23) return -1;
            break;
        default:
            return -1;
    }
	return 1000*(s+(m*60) + (h*3600));
}

void H4P_Timekeeper::setSchedule(H4P_SCHEDULE shed){
  for(auto s:shed){
    at(s.first,ON);
    at(s.second,OFF);
  }
}

std::string H4P_Timekeeper::strTime(uint32_t t){ // milliseconds!
	char buf[9];
    uint32_t sex=t/1000;
	sprintf(buf,"%02d:%02d:%02d",(sex%secsInDay())/3600,(sex/60)%60,sex%60);
	return std::string(buf);
}

std::string H4P_Timekeeper::strfTime(uint32_t t) { // This one uses seconds :)
	char buf[9];
	struct tm ts;
	time_t rt = t;
	ts = *gmtime(&rt);
	strftime( buf, sizeof(buf), "%X", &ts );
	return std::string(buf);
}

std::string H4P_Timekeeper::strfDate(uint32_t t) { // This one uses seconds :)
	char buf[11];
	struct tm ts;
	time_t rt = t;
	ts = *gmtime(&rt);
	strftime( buf, sizeof(buf), "%Y-%m-%d", &ts );
	return std::string(buf);
}

std::string H4P_Timekeeper::strfDateTime(char fmt[], uint32_t t) { // This one uses seconds :)
	char buf[40];
	struct tm ts;
	time_t rt = t;
	ts = *gmtime(&rt);
	strftime( buf, sizeof(buf), fmt, &ts );
	return std::string(buf);
}

void H4P_Timekeeper::svcUp(){
    sntp_init();
    if(!_mss00){
        h4.repeatWhile(
            [this]{ return !_mss00; },
            H4P_TIME_HOLDOFF,
            [this]{ sync(); },
            [this]{
                static bool gotRTC=false; 
                XLOG("NTP SYNC _mss00=%u %s",_mss00,CSTR(clockTime()));
                if(!gotRTC) {
                    H4Service::svcUp();
                    QEVENT(H4PE_RTC);
                    gotRTC=true;
                    h4.every(H4P_TIME_RESYNC,[this]{ sync(); },nullptr,H4P_TRID_SYNC,true); // tweakables
                }
            },
            H4P_TRID_TIME,
            true
        );
    } else H4Service::svcUp();
}

void H4P_Timekeeper::svcDown(){ 
    h4.cancelSingleton({H4P_TRID_TIME,H4P_TRID_SYNC});
	sntp_stop();
    H4Service::svcDown();
}

int H4P_Timekeeper::H4P_DST_EU(uint32_t t) {

	//*Test
//	struct tm x = {0, 0, 2, 25, 9, 120};
//	t = mktime(&x);

	time_t rt = t;
	struct tm ts = *gmtime(&rt);
	int y = ts.tm_year + 1900;

	//Bomb out if the year is invalid for this formula.
	if ((y<1996) || (y>2099)) {
		return 0;
	}
	
	// Construct the date for EU DST start.
	struct tm tmp = {0, 0, 1, 31 - (5*y/4 + 4) % 7, 2, y-1900};
	time_t st = mktime(&tmp);
	// Construct the date for EU DST end.
	tmp = {0, 0, 2, 31 - (5*y/4 + 1) % 7, 9, y-1900};
	time_t en = mktime(&tmp);
	
	//*Test
//	char buf[40];
//	struct tm tt;
//	tt = *gmtime(&st);
//	strftime( buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S", &tt );
//	Serial.printf("DST Start = %s\n", buf);
//	tt = *gmtime(&en);
//	strftime( buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S", &tt );
//	Serial.printf("DST End   = %s\n", buf);

	if ((t >= st) && (t < en)) {
		return 60*60;
	} else {
		return 0;
	}
	
	return -1;
	
}

int H4P_Timekeeper::H4P_DST_USA(uint32_t t) {

	//*Test
//	struct tm x = {0, 0, 2, 25, 9, 120};
//	t = mktime(&x);

	time_t rt = t;
	struct tm ts = *gmtime(&rt);
	int y = ts.tm_year + 1900;

	time_t en, st;
	
	//Formula for 2007 and later in USA.
	if (y>=2007) {
		// Construct the date for USA DST start.
		struct tm tmp = {0, 0, 1, 14 - (1 + y*5/4) % 7, 2, y-1900};
		st = mktime(&tmp);
		// Construct the date for USA DST end.
		tmp = {0, 0, 2, 7 - (1 + y*5/4) % 7, 10, y-1900};
		en = mktime(&tmp);
	}
	
	//Formula valid for 1900-2006, but was only used/adopted for 1950-1969 in USA.
	if (y<2007) {
		if ((y>=1950) && (y<=1969)) {
			// Construct the date for USA DST start.
			struct tm tmp = {0, 0, 1, (2+6*y-y/4) % 7+1, 3, y-1900};
			st = mktime(&tmp);
			// Construct the date for USA DST end.
			tmp = {0, 0, 2, 31-(y*5/4+1) % 7, 9, y-1900};
			en = mktime(&tmp);
		} else {
			return 0;
		}
	}
	
	//*Test
//	char buf[40];
//	struct tm tt;
//	tt = *gmtime(&st);
//	strftime( buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S", &tt );
//	Serial.printf("DST Start = %s\n", buf);
//	tt = *gmtime(&en);
//	strftime( buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S", &tt );
//	Serial.printf("DST End   = %s\n", buf);

	if ((t >= st) && (t < en)) {
		return 60*60;
	} else {
		return 0;
	}
	
	return -1;
	
}

void H4P_Timekeeper::tz(int tzOffset){
    svcDown();
    __HALsetTimezone(tzOffset);
    _mss00=0; // reset timekeeping, force sync
    svcUp();
}

std::string H4P_Timekeeper::upTime(){
	uint32_t t=millis();
	return stringFromInt(t / msInDay(),"%02d:")+strTime(t);
}