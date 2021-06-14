//#define H4P_VERBOSE 1
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,30,false) // Serial baud rate, larger Q size for scheduling, SerialCmd autostop

H4P_SCHEDULE mySchedule={
  {"07:00","09:00"}, // ON @ 0700, OFF @ 0900
  {"16:50","16:52"}, // ON @ 1300, OFF @ 1400
  {"18:00","23:00"}  // ON @ 1800, OFF @ 2300
};

//H4P_SerialLogger slog(H4PE_ALMOST_ALL &~ H4PE_HEARTBEAT &~ H4PE_GVCHANGE);
//H4P_Signaller h4fc;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","ticker"); // device defaults to "H4-<chip id>"
H4P_Timekeeper h4tk("192.168.1.4","0.fr.pool.ntp.org",60,H4P_Timekeeper::H4P_DST_EU); // 60 minutes cos France is GMT+1
H4P_AsyncMQTT h4mqtt("http://192.168.1.4:1883");
H4P_BinarySwitch h4onof(D6,ACTIVE_HIGH);
H4P_UPNPServer h4upnp("Timekeeper tester");
h4pMultifunctionButton h4mfb(0,INPUT_PULLUP,ACTIVE_LOW,15);
//H4P_RemoteUpdate h4ru("192.168.1.4:1880/update",__FILE__);

void onRTC(){
  Serial.printf("Clock set raw=%d pretty=%s\n",h4tk.msSinceMidnight(),CSTR(h4tk.clockTime()));
//  absolute time
  h4tk.at("04:20",ON);
  h4tk.at("04:20:30",OFF);
// same absolute time every day
  h4tk.daily("07:00",ON);
// relative time
// 'minutesFromNow' is a cheap hack so that the
// demo will always work no matter what time you run it
  h4tk.at(h4tk.minutesFromNow(1),ON);
  h4tk.at(h4tk.minutesFromNow(2),OFF);
// schedule arbitrary user function
  h4tk.setSchedule(mySchedule);
}

H4P_EventListener clok(H4PE_RTC | H4PE_ALARM,[](const std::string& svc,H4PE_TYPE t,const std::string& msg){
  switch(t){
    H4P_FUNCTION_ADAPTER_VOID(RTC);      
    H4P_ONOFF_CONNECTOR(ALARM); // connect alarm to default H4P_Binary[Switch|Thing]
  }
});

void h4setup(){
  h4.every(30000,[]{ Serial.printf("clock time: %s\n",CSTR(h4tk.clockTime())); });
}