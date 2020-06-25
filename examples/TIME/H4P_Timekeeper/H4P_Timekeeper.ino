#include<H4Plugins.h>
H4_USE_PLUGINS(115200,30,false) // Serial baud rate, larger Q size for scheduling, SerialCmd autostop

H4P_SCHEDULE mySchedule={
  {"07:00","09:00"}, // ON @ 0700, OFF @ 0900
  {"13:00","14:00"}, // ON @ 1300, OFF @ 1400
  {"18:00","23:00"}  // ON @ 1800, OFF @ 2300
};

void breakTime(bool onoff){ Serial.printf("Break time%s!\n",onoff ? "":" over"); }

void onRTC(){
  Serial.printf("Clock set raw=%d pretty=%s\n",h4tk.msSinceMidnight(),CSTR(h4tk.clockTime()));
//  These can ONLY be included here...
//  1) they should run 1x only
//  2) they WILL NOT WORK unless the clock has been successfully sync'd!
//
//  absolute time - call arbitrary user function
  h4tk.at("04:20",ON,breakTime);
  h4tk.at("04:20:30",OFF,breakTime);
//  absolute time - call BinarySwitch
  h4tk.atSource("12:15",ON);
  h4tk.atSource("12:30",OFF);
// same absolute time every day - call arbitrary user function
  h4tk.daily("07:00",ON,[](bool onoff){ Serial.printf("WAKEY, WAKEY!!\n"); });
// same absolute time every day - call BinarySwitch
  h4tk.dailySource("07:01",ON);

// relative time - call BinaryThing/ Switch
// 'minutesFromNow' is a cheap hack so that the
// demo will always work no matter what time you run it
// DO NOT USE THIS YOURSELF: h4.once(60000... does exactly the same but
// much more efficiently!
  h4tk.atSource(h4tk.minutesFromNow(1),ON);
  h4tk.atSource(h4tk.minutesFromNow(2),OFF);

// schedule arbitrary user function
  h4tk.setSchedule(mySchedule,breakTime);
// schedule BinarySwitch
  h4tk.setScheduleSource(mySchedule);
}

H4P_GPIOManager h4gm;
H4P_FlasherController h4fc;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX"); // device defaults to "H4-<chip id>"
H4P_AsyncWebServer h4asws;
H4P_Timekeeper h4tk("0.fr.pool.ntp.org","1.fr.pool.ntp.org",60,H4P_Timekeeper::DST_EU); // 60 minutes cos France is GMT+1
H4P_AsyncMQTT h4mqtt("192.168.1.4",1883);
H4P_BinarySwitch h4onof(RELAY_BUILTIN,ACTIVE_HIGH,OFF);
H4P_UPNPServer h4upnp; // friendly name defaults to "upnp <chip id>"
H4P_MultiFunctionButton h4mfb(BUTTON_BUILTIN,INPUT,ACTIVE_LOW,15,LED_BUILTIN,ACTIVE_LOW);
H4P_RemoteUpdate h4ru("http://192.168.1.4:1880/update");

void h4setup(){
  h4.every(30000,[]{ Serial.printf("clock time: %s\n",CSTR(h4tk.clockTime())); });
}