#define H4P_VERBOSE 1
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

IPAddress htcIP(192,168,1,252);

h4pRoamingIP htc("HTC",htcIP);
h4pRoamingIP elvis("Elvis","192.168.1.31");

h4pRoamingH4 crow("squawk");
h4pRoamingH4 master("master");
h4pRoamingH4 landing("landing");
h4pRoamingH4 salon("salon");
h4pRoamingH4 clok("ntpclock");

h4pRoamingDotLocal dot("H3","smb","tcp");

h4pRoamingUPNP earth("middle","USN","uuid:Socket-1_0-upnpF9198B");

int occupancy=0;

H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","detector");
H4P_BinarySwitch h4onof(D6,ACTIVE_HIGH,OFF);

void onPRESENCE(const string& who,bool b){
  if(b) occupancy++;
  else occupancy--; 
  Serial.printf("%s has %s the building - occupancy now %d\n",CSTR(who),b ? "entered":"left",occupancy);
  h4onof.turn(occupancy); // will the last person to leave please turn off the lights... :)
}

H4P_EventListener chchchanges(H4PE_PRESENCE,[](const string& svc,H4PE_TYPE t,const string& msg){ 
  switch(t){
    H4P_FUNCTION_ADAPTER_PRESENCE
  }
});
