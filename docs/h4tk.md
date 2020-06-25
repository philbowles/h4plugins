![H4P Flyer](../assets/WiFiLogo.jpg)

# NTP Synchronisation (short name="time")

## Adds real-time alarms and scheduling (**EXPERIMENTAL** Sunrise/Sunset times)

Many thanks to Adam Sharp (http://threeorbs.co.uk) for extensive work and help on this plugin

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---

# What does it do?

Syncrhonises with two user-chosen public NTP servers. At boot time, it reads the current time then continues to maintain an internal clock. Thus even if WiFi later becomes unavailble, the "Wall-clock time" will always be available, even though it may drift.

The internal clock is re-synchronised 1x per hour (configurable) while ever a network connection is available

---

# Usage

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop
H4P_Timekeeper h4tk(...
```

## Dependencies

* [H4P_WiFi](h4wifi.md) Plugin

## Commands Added

* h4/time/change/x,y (payload x,y = ntp1,ntp2)
* h4/time/sync (force re-sync)
* h4/time/tz/x (payload x = *integer* offest of timezeone in minutes from GMT)

## Callbacks

```cpp
void onRTC(void); // called when a valid time sync has been mad with one of the servers
// to use any or scheduling functions, you MUST declare this in your sketch and do
// all of you alrms / schedules inside it, otherwise they will simply do nothing if called
// when there is no valid NTP time yet

void onAlarm(bool); // user-defined function called when alarm occurs
```

## Data Structures

```cpp
using H4P_DURATION = pair<string,string>; // ON/OFF times
using H4P_SCHEDULE = vector<H4P_DURATION>; // schedule of ON/OFF times
/* e.g.
H4P_SCHEDULE mySchedule={
  {"07:00","09:00"}, // ON @ 0700, OFF @ 0900
  {"13:00","14:00"}, // ON @ 1300, OFF @ 1400
  {"18:00","23:00"}  // ON @ 1800, OFF @ 2300
};
*/
```

---

# API

```cpp
/*
*Note: All internal real times are from EPOCH and calculated in seconds.  This
keeps everything functional with integer maths, but allows for a fairly fine
resolution within the proceedings.

*Note: Dates are ISO = YYYY-MM-DD which is country independant.

*/
// Constructor
// NTP servers can be either domain name or text ip address e.g. "192.168.1.4"
// tzOffset = *integer* number of seconds offset from GMT: This means users in Newfoundland, India, Iran, Afghanistan
// Myanmar, Sri Lanka, the Marquesas, parts of Australia, Nepal, and the Chatham Islands... can now set the correct TZ
// fDST = the (optional) name of a daylight saving function: either DST_EU or DST_USA (see below)
H4P_Timekeeper(const string& ntp1,const string& ntp2,int tzOffset=0,H4_FN_DST fDST=nullptr);

// Common Parameters:
// when is the desired time in "hh:mm:ss" or simply "hh:mm" format
// onoff = either ON or OFF: the desired state of the BinarySwith/Thing at time<when>
// sched = list of ON/OF pairs (see 'Data Structures' above
// onAlarm is an arbitrary user-defined function taking a single bool parameter 
void at(const string& when,bool onoff,H4BS_FN_SWITCH onAlarm); //  call onAlarm(onoff) at "hh:mm:ss"
void atSource(const string& when,bool onoff); // Switch BinarySwitch/Thing on/off at "hh:mm:ss"
void change(const string& ntp1,const string& ntp2); // change ntp servers
string clockTime(); // return "hh:mm:ss" representing curretn "Wall-Clock Time"
uint32_t clockEPOCH(); // Current number of seconds since EPOCH.
uint32_t clockEPOCHLocal(); // Current number of seconds since EPOCH plus TZ offset.
string clockStrTimeLocal(); // Current time + TZ as string HH:MM:SS.
void daily(const string& when,bool onoff,H4BS_FN_SWITCH onAlarm); // call onAlarm(onoff) at "hh:mm:ss" every day
void dailySource(const string& when,bool onoff); // Switch BinarySwitch/Thing on/off at "hh:mm:ss" every day
uint32_t msSinceMidnight(); // return number of milliseconds since 00:00
int parseTime(const string& ts); // given "hh:mm" or "hh:mm:ss", returns milliseconds value of the time, or -1 if invalid
void setSchedule(H4P_SCHEDULE sched,H4BS_FN_SWITCH onAlarm); //  call onAlarm(onoff) according to pre-defined schedule
void setScheduleSource(H4P_SCHEDULE sched); // Switch BinarySwitch/Thing on/off according to pre-defined schedule
string strfTime(uint32_t t); //Convert the supplied number of seconds since EPOCH as a time string
string strfDate(uint32_t t); //Convert the supplied number of seconds since EPOCH as a date string
string strfDateTime(char fmt[] = "%a %Y-%m-%d %H:%M:%S", uint32_t t=0); //Convert the supplied number of seconds into the supplied format.
string strTime(uint32_t t); // returns "hh:mm:ss" string representation t milliseconds
void sync(); // forces re-sync whith NTP servers (normally done automatically 1x per hour)
void tz(uint32_t tzOffset); // change timezone and force resync for SUMMER/WINTER time. Will screw up any existing timers!
string upTime(); // returns "hh:mm:ss" value of time since bootup
// daylight saving
int DST_EU(uint32_t t); // DST offset for t in EU (inc UK).
int DST_USA(uint32_t t); // DST offset for t in USA.

```

---

# Sunrise/Sunset

Calls the public API @ https://api.sunrise-sunset.org/json with your Lat / Long and returns sunrise / sunset times

---

# Usage

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop
H4P_Sunrise h4ss(...
```

## Dependencies

* [H4P_WiFi](h4wifi.md) Plugin
* H4P_Timekeeper Plugin

## Commands Added

None

## Callbacks

```cpp
void onAlarm(bool); // user-defined function called when alarm occurs
```

---

# API

```cpp
// Constructor
// lat / long must be given as string representaon of floating point values, e.g. "48.4738943","-0.6029038"
// H4P_EPHEMERA riseOrSet is either SUNRISE or SUNSET
H4P_Sunrise(const string& latitude,const string& longitude);

void  at(H4P_EPHEMERA riseOrSet,bool onoff,H4BS_FN_SWITCH onAlarm); //  call onAlarm(onoff) at SUNRISE or SUNSET
void  atSource(H4P_EPHEMERA riseOrSet,bool onoff); // Switch BinarySwitch/Thing on/off at SUNRISE or SUNSET
```

[Example sketch](../examples/TIME/H4P_SunriseSunset/H4P_SunriseSunset.ino)

---

(c) 2020 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Blog](https://8266iot.blogspot.com)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook IOT with ESP8266 (moderator)}](https://www.facebook.com/groups/1591467384241011/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)