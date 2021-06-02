#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

int occupancy=0;

h4pRoamingIP elvis("Elvis","192.168.1.31");

H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","detector");
H4P_BinarySwitch h4onof(D6,ACTIVE_HIGH,H4P_UILED_BI,ON);

H4P_EventListener chchchanges(H4PE_PRESENCE,[](const std::string& svc,H4PE_TYPE t,const std::string& msg){ 
  switch(t){
    H4P_ONOFF_CONNECTOR_INVERTED(PRESENCE)
  }
});
