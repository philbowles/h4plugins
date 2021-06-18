#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) 

#define U_POLL_FREQ     1000
#define N_SAMPLES          5
#define U_HYSTERESIS      10 // minimum amout of difference between two samples before "changed" is declared. Prevents "flutter"

H4P_EventListener gpio(H4PE_GPIO,[](const std::string& pin,H4PE_TYPE t,const std::string& msg){
    int p=atoi(pin.c_str());
    int v=atoi(msg.c_str());
    switch(p){
      case A0:
        Serial.printf("P%d Value=%d\n",p,v);
        break;
    }
});

// Reports every F mSec if value has changed by more than H since last F period
h4pAnalogPolled ap(A0,U_POLL_FREQ,U_HYSTERESIS);
//
// Reports every F mSec * N_SAMPLES taking arithmetic mean of N_SAMPLES
//h4pAnalogAverage aa(A0,U_POLL_FREQ,N_SAMPLES);
//
// Reports every F mSec * N_SAMPLES taking arithmetic mean of N_SAMPLES 
// BUT ONLY IF CHANGED (BY MORE THAN H) FROM PREVIOUS READING
//h4pAnalogAvgChanged aac(A0,U_POLL_FREQ,N_SAMPLES,U_HYSTERESIS);
//
// Reports every F mSec taking arithmetic mean of "sliding window" of last N_SAMPLES
//h4pAnalogAvgWindow aaw(A0,U_POLL_FREQ,N_SAMPLES);
//
// Reports every F mSec calcualtiong cumulative moving ("rolling") average
//h4pAnalogAvgRolling aar(A0,U_POLL_FREQ);
//
// Converts analog value to Celsius temperature assuming TMP36 is connected
//h4pAnalogTMP36 tmp236(A0,U_POLL_FREQ,U_HYSTERESIS);
//
// Reports every F mSec if value is > (H4PM_GREATER) or < (H4PM_LESS) (+/- hysteresis) chosen value. Here it's 100
//h4pAnalogThreshold aat(A0,U_POLL_FREQ,H4PM_GREATER,100,U_HYSTERESIS);