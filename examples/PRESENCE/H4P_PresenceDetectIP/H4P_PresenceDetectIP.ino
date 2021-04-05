#define H4P_VERBOSE 1
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

void onPRESENCE(const string& who,bool b){
  Serial.printf("%s has %s the building\n",CSTR(who),b ? "entered":"left");
}

H4P_EventListener chchchanges(H4PE_PRESENCE,[](const string& svc,H4PE_TYPE t,const string& msg){ 
  switch(t){
    H4P_FUNCTION_ADAPTER_SI(PRESENCE);
  }
});

IPAddress htcIP(192,168,1,252);

h4pRoamingIP htc("HTC",htcIP);
h4pRoamingIP elvis("Elvis","192.168.1.31");

H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","detector");
H4P_BinarySwitch h4onof(D6,ACTIVE_HIGH,OFF);