#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) 

h4pEncoder rotary(D1,D2);

void onGPIO(int pin,int value){ Serial.printf("P%d V%d\n",pin,value); }
void h4pGlobalEventHandler(const std::string& svc,H4PE_TYPE t,const std::string& msg){ switch(t){ H4P_FUNCTION_ADAPTER_GPIO; } }
