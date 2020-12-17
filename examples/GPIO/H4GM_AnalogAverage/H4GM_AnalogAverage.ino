#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop
/*
My major testing devices were nodeMCU which has builtin button on GPIO0 which is ACTIVE_LOW
and STM32NUCLEO-F429ZI whuch has a user button that is ACTIVE_HIGH

You will probably need to adjust these values for you own device
*/

#define USER_BTN 0
#define UB_ACTIVE ACTIVE_LOW
#define UL_ACTIVE ACTIVE_LOW
/*
    ALL GPIO strategies are derived from H4GPIOPin: the following members are available
    inside ALL GPIO pin callbacks, once you have a valid pointer for the pin type using 
    H4GM_PIN( type ) with specific underlying type Raw needs H4GM_PIN(Raw);

    uint8_t         pin=0;                  // GPIO hardware pin number
    uint8_t         gpioType=0;             // INPUT, INPUT_PULLUP, OUTPUT etc
    H4GM_STYLE      style;                  // Strategy: Raw, debounced, retriggering etc
    uint8_t         sense=0;                // active HIGH or LOW
    unsigned long   Tevt=0;                 // uS time of last event
    int             state=0;                // 32 wide as it holds analog value as well as digital 0/1
                                            // and not uint because encoder returns -1 or +1
    uint32_t        delta=0;                // uS since last change
    uint32_t        rate=0;                 // instantaenous rate cps
    uint32_t        Rpeak=0;                // peak rate   
    uint32_t        cps=0;                  // sigma changes/sec (used internally, frequently re-set)
    uint32_t        cMax=UINT_MAX;          // max permitted cps (see "throttling");
    uint32_t        nEvents=UINT_MAX;       // sigma "Events" (meaning depends on inheriting strategy)
            
    Additional members for AnalogAverage:

        nSamples       the number of samples to read every <freq> mS before sending the average of those N samples
        
        AnalogAverage has its state checked periodically, 

*/
        
H4P_GPIOManager h4gm;

#define U_POLL_FREQ     1000
#define N_SAMPLES          5

void h4setup() { // H4 constructor starts Serial
    Serial.println("H4P_GPIOManager AnalogAverage Example v"H4P_VERSION);

    h4gm.AnalogAverage(A0,U_POLL_FREQ,N_SAMPLES,[](H4GPIOPin* ptr){
        H4GM_PIN(AnalogAverage); // Create the correct pointer type in 'pin'
        Serial.print("T=");Serial.print(millis());
        Serial.print(" AnalogAverage ");Serial.print(pin->pin);
        Serial.print(" AnalogAverage IS ");Serial.println(pin->logicalRead());
    });
}
