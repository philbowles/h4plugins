#define H4P_VERBOSE 1
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

h4pRoamingUPNP crow("squawk","USN","uuid:Socket-1_0-upnp0BC939");
h4pRoamingUPNP master("master","USN","uuid:Socket-1_0-upnpF906B3");
h4pRoamingUPNP earth("middle","USN","uuid:Socket-1_0-upnpF9198B");

int occupancy=0;

IPAddress htcIP(192,168,1,252);

H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","detector");
H4P_BinarySwitch h4onof(D6,ACTIVE_HIGH);

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