
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
#include<H4P_SerialCmd.h>
#include<H4P_BinaryThing.h>

void __attribute__((weak)) onRTC(){}

constexpr uint32_t secsInDay(){ return 86400; }
constexpr uint32_t msInDay(){ return 1000*secsInDay(); }

H4P_Timekeeper::H4P_Timekeeper(const string& ntp1,const string& ntp2,int tzo,H4_FN_DST fDST): _fDST(fDST), H4Plugin(timeTag()){
    _cmds={
        {_pName,   { H4PC_H4, _subCmd, nullptr}}, // root for this plugin
        {"change", { _subCmd,       0, CMDVS(_change)}},
        {"sync",   { _subCmd,       0, CMD(sync)}},
        {"tz",     { _subCmd,       0, CMDVS(_tz)}}
    };
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
//    Serial.printf("**************** SYNC S=%u tzo=%d %s\n",stamp,_tzo,sntp_get_real_time(stamp));
	if(stamp > 30000){ // 28800 +leeway: default is GMT+8
        stamp+=(_tzo*60);
		vector<string> dp=split(replaceAll(sntp_get_real_time(stamp + _fDST(stamp)),"  "," ")," ");
//        dumpvs(dp);
        _mss00=parseTime(dp[3])-millis();
//        Serial.printf("**************** SYNC S=%u tzo=%d %s ms00=%u\n",stamp,_tzo,sntp_get_real_time(stamp + _fDST(stamp)),_mss00);
	}
}
#else 

void H4P_Timekeeper::__HALsetTimezone(int tzo){
    _tzo=tzo;
    string tz="GMT"+stringFromInt((-1*tzo),"%+02d");
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

uint32_t H4P_Timekeeper::__alarmCore (vector<string> vs,bool daily,H4BS_FN_SWITCH f){
    vector<string> vg=split(H4PAYLOAD,",");
    if(vg.size()>2) return H4_CMD_TOO_MANY_PARAMS;
    if(vg.size()<2) return H4_CMD_TOO_FEW_PARAMS;
    int T=parseTime(vg[0]);
    if(T<0) return H4_CMD_PAYLOAD_FORMAT;
    string b=vg[1];
    if(!isNumeric(b)) return H4_CMD_NOT_NUMERIC;
    int onoff=atoi(CSTR(b));
    if(_mss00){ // onRTC
        H4EVENT("%s %s -> %s",daily ? "Daily":"S/Shot",CSTR(strTime(T)),onoff ? "ON":"OFF");
        int msDue=T-msSinceMidnight();
        if(msDue < 0) msDue+=msInDay();
        uint32_t u=daily ? H4P_TRID_DALY:H4P_TRID_SHOT;
        h4.add([this,f,onoff]{
            if(_mss00) f(onoff);
            //else H4EVENT("RTC ignored"); // don't fire if NTP not sync'ed 
        },msDue,daily ? msDue:0,H4Countdown(1),nullptr,TAG(daily ? 7:3));
        return H4_CMD_OK;
    } //else H4EVENT("Alarm ignored"); // don't set if NTP not sync'ed
    return H4_CMD_NOT_NOW; 
}

uint32_t H4P_Timekeeper::_at(vector<string> vs){ return __alarmCore(vs,false,[this](bool b){ _btp->turn(b); }); }

uint32_t H4P_Timekeeper::_change(vector<string> vs){ return guardString2(vs,[this](string a,string b){ change(a,b); return H4_CMD_OK; }); }

uint32_t H4P_Timekeeper::_daily(vector<string> vs){ return __alarmCore(vs,true,[this](bool b){ _btp->turn(b); }); }

void H4P_Timekeeper::_hookIn(){ 
    DEPEND(wifi);
    H4Plugin* p=isLoaded(onofTag());
    if(p) {
        _btp=reinterpret_cast<H4P_BinaryThing*>(p);
        h4cmd.addCmd("at",_subCmd,0,CMDVS(_at));
        h4cmd.addCmd("daily",_subCmd,0,CMDVS(_daily));
    }
}

void H4P_Timekeeper::_setupSNTP(const string& ntp1, const string& ntp2){
    _ntp1=ntp1;
    _ntp2=ntp2;
    sntp_setservername(0,(char*) _ntp1.c_str());
    sntp_setservername(1,(char*) _ntp2.c_str());
}

void H4P_Timekeeper::_start(){
    sntp_init();
    if(!_mss00){
        h4.repeatWhile(
            [this]{ return !_mss00; },
            H4P_TIME_HOLDOFF,
            [this]{ sync(); },
            [this]{
                static bool gotRTC=false; 
                H4EVENT("NTP SYNC _mss00=%u %s",_mss00,CSTR(clockTime()));
                if(!gotRTC) {
                    _upHooks();
                    onRTC();
                    gotRTC=true;
                    h4.every(H4P_TIME_RESYNC,[this]{ sync(); },nullptr,H4P_TRID_SYNC,true); // tweakables
                }
            },
            H4P_TRID_TIME,
            true
        );
    } else _upHooks();
}

void H4P_Timekeeper::_stop(){ 
    h4.cancelSingleton({H4P_TRID_TIME,H4P_TRID_SYNC});
	sntp_stop();
    _downHooks();
}

uint32_t H4P_Timekeeper::_tz(vector<string> vs){
    return guardInt1(vs,[this](int v){
        tz(v); 
        return H4_CMD_OK;
    });
}
//
//
//
void H4P_Timekeeper::at(const string& when,bool onoff,H4BS_FN_SWITCH f){ __alarmCore({when+","+stringFromInt(onoff)},false,f); }

void H4P_Timekeeper::atSource(const string& when,bool onoff){ if(_btp) _at({when+","+stringFromInt(onoff)}); }

void H4P_Timekeeper::change(const string& ntp1,const string& ntp2){ _setupSNTP(ntp1,ntp2); }

void H4P_Timekeeper::daily(const string& when,bool onoff,H4BS_FN_SWITCH f){ __alarmCore({when+","+stringFromInt(onoff)},true,f); }

void H4P_Timekeeper::dailySource(const string& when,bool onoff){ if(_btp) _daily({when+","+stringFromInt(onoff)}); }

string H4P_Timekeeper::minutesFromNow(uint32_t m){
    string longtime=h4tk.strTime((msSinceMidnight())+(m*60000));  
    return string(longtime.begin(),longtime.end()-3); 
}

int H4P_Timekeeper::parseTime(const string& ts){ // in milliseconds!
//    Serial.printf("parseTime %s\n",CSTR(ts));
	vector<string> parts=split(ts,":");
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

void H4P_Timekeeper::setSchedule(H4P_SCHEDULE shed,H4BS_FN_SWITCH f){
  for(auto s:shed){
    h4tk.at(s.first,ON,f);
    h4tk.at(s.second,OFF,f);    
  }
}
// optimise with _btpON lambda?
void H4P_Timekeeper::setScheduleSource(H4P_SCHEDULE shed){
  for(auto s:shed){
    h4tk.atSource(s.first,ON);
    h4tk.atSource(s.second,OFF);
  }
}

void H4P_Timekeeper::show(){
//    reply("_mss00=%d, ms since 00:00=%d",_mss00,msSinceMidnight());
    reply("TZO=%d",_tzo);
    reply("%s %s",CSTR(_ntp1),CSTR(_ntp2));
    reply("Wallclock=%s, UpTime=%s",CSTR(clockTime()),CSTR(upTime()));
}

string H4P_Timekeeper::strTime(uint32_t t){ // milliseconds!
	char buf[9];
    uint32_t sex=t/1000;
	sprintf(buf,"%02d:%02d:%02d",(sex%secsInDay())/3600,(sex/60)%60,sex%60);
	return string(buf);
}

string H4P_Timekeeper::strfTime(uint32_t t) { // This one uses seconds :)
	char buf[9];
	struct tm ts;
	time_t rt = t;
	ts = *gmtime(&rt);
	strftime( buf, sizeof(buf), "%X", &ts );
	return string(buf);
}

string H4P_Timekeeper::strfDate(uint32_t t) { // This one uses seconds :)
	char buf[11];
	struct tm ts;
	time_t rt = t;
	ts = *gmtime(&rt);
	strftime( buf, sizeof(buf), "%Y-%m-%d", &ts );
	return string(buf);
}

string H4P_Timekeeper::strfDateTime(char fmt[], uint32_t t) { // This one uses seconds :)
	char buf[40];
	struct tm ts;
	time_t rt = t;
	ts = *gmtime(&rt);
	strftime( buf, sizeof(buf), fmt, &ts );
	return string(buf);
}

int H4P_Timekeeper::DST_EU(uint32_t t) {

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

int H4P_Timekeeper::DST_USA(uint32_t t) {

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
    _stop();
    __HALsetTimezone(tzOffset);
    _mss00=0; // reset timekeeping, force sync
    _start();
}

string H4P_Timekeeper::upTime(){
	uint32_t t=millis();
	return stringFromInt(t / msInDay(),"%02d:")+strTime(t);
}