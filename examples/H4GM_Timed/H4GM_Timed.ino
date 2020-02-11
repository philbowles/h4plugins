#include<H4Plugins.h>
H4_USE_PLUGINS

H4 h4(115200,20); //auto-start Serial @ 115200, Q size=20 

H4P_GPIOManager h4gm;
H4P_FlasherController h4fc;
/*
My major testing devices were nodeMCU which has builtin button on GPIO0 which is ACTIVE_LOW
and STM32NUCLEO-F429ZI whuch has a user button that is ACTIVE_HIGH

You will probably need to adjust these values for you own device
*/
#ifdef ARDUINO_ARCH_STM32
  #define UB_ACTIVE ACTIVE_HIGH
  #define UL_ACTIVE ACTIVE_HIGH
#else
  #define USER_BTN 0
  #define UB_ACTIVE ACTIVE_LOW
  #define UL_ACTIVE ACTIVE_LOW
#endif
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
            
    Additional members for Timed:

        uint32_t    held // number of microseconds button was held ON

        Timed strategy counts how long it is held in the ON position. When released back to the OFF state
        it has the number of microseconds button was held ON for in the extra variable 'held'

        It is derived form the Debounced strategy, so it only changes state on a "clean"
        transition. For this reason it also requires a switch-dependent value for dbTimeMs

*/
// Debouncing time in mS
#define U_DBTIME_MS    12

void h4setup() { // H4 constructor starts Serial
    Serial.println("H4P_GPIOManager Timed Example v"H4P_VERSION);
    Serial.print("GPIO ");Serial.print(USER_BTN);Serial.print(" ACTIVE ");Serial.println(UB_ACTIVE ? "HIGH":"LOW");

    h4gm.Output(LED_BUILTIN,UB_ACTIVE,OFF); // set LED to OFF

    h4gm.Timed(USER_BTN,INPUT,UB_ACTIVE,U_DBTIME_MS,[](H4GPIOPin* ptr){
            H4GM_PIN(Timed); // Create the correct pointer type in 'pin'
            h4gm.logicalWrite(LED_BUILTIN,pin->state);
            if(!pin->state) {
                Serial.print("LED WAS ON FOR : ");Serial.print(pin->held);Serial.println(" microseconds"); // low-level pin times are uS
            }
        });
}
