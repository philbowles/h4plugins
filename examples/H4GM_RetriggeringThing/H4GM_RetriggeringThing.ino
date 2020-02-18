#include<H4Plugins.h>
H4_USE_PLUGINS
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
            
    Additional members for Retriggering:

    none

    Think of a PIR movement sensor: On first detection it goes ON and stays on for a chosen period (the "timeout").
    Any *new* movement before the timout period expires will cause it to start all over again, staying ON and 
    re-starting the clock or "re-triggering the timeout".

    The Retriggering strategy behaves identically. You get a callback when the pin changes to ON and then again
    to OFF when `timeout` milliseconds after the final physical retriggering event have elapsed. 

    New in v0.0.3.4 is the "RetriggeringThing" which "ties" or links" or "binds" the Retriggering pin to any kind
    of xSwitch or xThing

*/
#define U_TIMEOUT       10000


H4 h4(115200,20); //auto-start Serial @ 115200, Q size=20 

H4P_GPIOManager h4gm;
H4P_BinarySwitch h4bt(LED_BUILTIN,UL_ACTIVE,OFF);
// or e.g. H4P_BinaryThing, UPNPThing, UPNPSwitch - in fact any xSwitch or xThing

void h4setup() { // H4 constructor starts Serial
    Serial.println("H4P_GPIOManager Retriggering Example v"H4P_VERSION);
    Serial.print("GPIO ");Serial.print(USER_BTN);Serial.print(" ACTIVE ");Serial.println(UB_ACTIVE ? "HIGH":"LOW");

    h4gm.RetriggeringThing(USER_BTN,INPUT,UB_ACTIVE,U_TIMEOUT,&h4bt);
}
