#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) 

H4P_BinarySwitch bs(D6,ACTIVE_HIGH);
h4pLatching latch(0,INPUT_PULLUP,ACTIVE_LOW,20,new npPUBLISHONOFF);
