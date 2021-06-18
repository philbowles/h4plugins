#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

h4pRoamingH4 crow("squawk");
h4pRoamingH4 master("master");
h4pRoamingH4 earth("middle");
h4pRoamingH4 landing("landing");
h4pRoamingH4 salon("salon");
h4pRoamingH4 clok("ntpclock");

h4pRoamingDotLocal dot("H3","smb","tcp"); // my satellite TV receiver
h4pRoamingDotLocal box("box","http","tcp"); // my main pc

h4pRoamingIP htc("HTC","192.168.1.252");
h4pRoamingIP elvis("Elvis","192.168.1.31");

int occupancy=0;

H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","detector");
H4P_BinarySwitch h4onof(D6,ACTIVE_HIGH,OFF);

void onPRESENCE(const std::string& who,bool b){
  if(b) occupancy++;
  else occupancy--; 
  Serial.printf("%s has %s the building - occupancy now %d\n",CSTR(who),b ? "entered":"left",occupancy);
  h4onof.turn(occupancy); // will the last person to leave please turn off the lights... :)
}

H4P_EventListener chchchanges(H4PE_PRESENCE,[](const std::string& svc,H4PE_TYPE t,const std::string& msg){ 
  switch(t){
    H4P_FUNCTION_ADAPTER_PRESENCE
  }
});