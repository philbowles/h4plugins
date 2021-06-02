#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","eighteen");
H4P_BinarySwitch h4onof(D6,ACTIVE_HIGH);

h4pRoamingDotLocal dot("H3","smb","tcp");

H4P_EventListener chchchanges(H4PE_PRESENCE | H4PE_GRID,[](const std::string& svc,H4PE_TYPE t,const std::string& msg){ 
  switch(t){
    H4P_ONOFF_CONNECTOR(PRESENCE) // will make h4onof follow in/out status
  }
});