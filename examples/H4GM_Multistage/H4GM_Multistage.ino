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
            
    Additional members for Multistage:

        uint32_t        stage // current "held" stage (depends on user-supplied stageMap)
        uint32_t        held // how long button hs been held ON in microseconds

        Using a Multistage GPIO can *look* a little complex, but the concept is simple: perform different
        actions depending on how long the button is held down. It helps if you have already run the
        Repeating and Timed examples.

        You supply a table of times and functions (the stageMap) and Multistage does the rest. As each
        time point is passed, it calls a your progress routine so you can do other things to reflect the
        increasinghold time: in this example, a flashing LED gets faster the longer the button is held.

*/
// Debouncing time in mS
#define U_DBTIME_MS    12

void stage0(H4GPIOPin* ptr){// run this if button held less than 2 seconds (2000ms)
    H4GM_PIN(Multistage); // Create the correct pointer type in 'pin'
    Serial.print("Stage 0 (short press) ");Serial.print(pin->stage);Serial.print(" held ");Serial.println(pin->held);
}
void stage1(H4GPIOPin* ptr){// run this if button held between 2 and 5 seconds
    H4GM_PIN(Multistage); // Create the correct pointer type in 'pin'
    Serial.print("Stage 1 (medium press) ");Serial.print(pin->stage);Serial.print(" held ");Serial.println(pin->held);
    h4fc.stopLED(LED_BUILTIN);
}
void stage2(H4GPIOPin* ptr){// run this if button held longer than 5 seconds
    H4GM_PIN(Multistage); // Create the correct pointer type in 'pin'
    Serial.print("Stage 2 (long press) ");Serial.print(pin->stage);Serial.print(" held ");Serial.println(pin->held);
    h4fc.stopLED(LED_BUILTIN);
}

void progress(H4GPIOPin* ptr){ // run this as each stage changes
    H4GM_PIN(Multistage); // Create the correct pointer type in 'pin'
    Serial.print("progress ");Serial.print(pin->stage);Serial.print(" held ");Serial.println(pin->held);
    switch(pin->stage){
        case 1: // over 2 seconds, medium flash
            h4fc.flashLED(250,LED_BUILTIN,UL_ACTIVE);     
            break;
        case 2: // over 5 seconds, fast flash
            h4fc.flashLED(50,LED_BUILTIN,UL_ACTIVE);     
            break;
        default: // do nothing if less than 2 seconds
            break;
    }
}
//
//  This is the heart of the Multistage strategy:  a table of times+functions
//
H4GM_STAGE_MAP stageMap={
    {0,stage0},    // if button held less than 2 seconds (2000ms) call stage0 when released
    {2000,stage1}, // if button held between 2 and 5 seconds call stage1 when released
    {5000,stage2} // if button held longer than 5 seconds call stage2 when released
};

void h4setup() { // H4 constructor starts Serial
    Serial.println("H4P_GPIOManager Multistage Example v"H4P_VERSION);
    Serial.print("GPIO ");Serial.print(USER_BTN);Serial.print(" ACTIVE ");Serial.println(UB_ACTIVE ? "HIGH":"LOW");

    // Lots of setup, now a bit of an anti-climax! 
    h4gm.Multistage(USER_BTN,INPUT,UB_ACTIVE,U_DBTIME_MS,stageMap,progress);
}