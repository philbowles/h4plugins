#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) 

#define USER_BTN 0
#define UB_ACTIVE ACTIVE_LOW
#define UB_MS 15
// 15mSec of debounce time. This will vary for all hardware: experiment for the best value

int      state=0;

h4pDebounced flat(USER_BTN,INPUT_PULLUP,UB_ACTIVE,UB_MS,new npUPDATEINT{state});

void h4setup(){
  h4.every(1000,[]{ Serial.printf("Value of state integer is %d\n",state); });
}
