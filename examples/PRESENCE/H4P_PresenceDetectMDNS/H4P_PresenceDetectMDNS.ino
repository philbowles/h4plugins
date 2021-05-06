#define H4P_VERBOSE 1
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

void onPRESENCE(const string& who,const string& ip){
  Serial.printf("%s has %s the building on IP %s\n",CSTR(who),ip.size() ? "entered":"left",CSTR(ip));
}
/*
void onGRID(const string& who,const string& ip){
  Serial.printf("FELLOW H4 %s has %s the network on IP %s\n",CSTR(who),ip.size() ? "entered":"left",CSTR(ip));
  Serial.printf(" H4 Family:\n");
  for(auto const& g:h4pGrid) Serial.printf("  %s %s\n",CSTR(g.first),CSTR(g.second));  
}
*/
H4P_SerialLogger slog;

H4P_EventListener chchchanges(H4PE_PRESENCE | H4PE_GRID,[](const string& svc,H4PE_TYPE t,const string& msg){ 
  switch(t){
    H4P_FUNCTION_ADAPTER(PRESENCE);
//    H4P_FUNCTION_ADAPTER(GRID);
  }
});

H4P_EmitHeap umble;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","eighteen");
H4P_AsyncMQTT h4mqtt("http://192.168.1.4:1883");
H4P_MQTTLogger eavy("heap",H4PE_HEAP);
H4P_BinarySwitch h4onof(D6,ACTIVE_HIGH,OFF);
H4P_UPNPServer up("Sherlock Holmes");
/*
H4P_Gatekeeper grid;

h4pRoamingDotLocal dot("H3","smb","tcp");
h4pRoamingH4 clok("ntpclock");
h4pRoamingIP elvis("Elvis","192.168.1.31");
//h4pRoamingUPNP crow("squawk","USN","uuid:Socket-1_0-upnp0BC939");
h4pRoamingUPNP master("master","USN","uuid:Socket-1_0-upnpF906B3");
*/