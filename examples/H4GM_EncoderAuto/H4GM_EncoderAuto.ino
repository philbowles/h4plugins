#include<H4Plugins.h>
H4_USE_PLUGINS

H4 h4(115200,20); //auto-start Serial @ 115200, Q size=20 

H4P_GPIOManager h4gm;
/*
My major testing devices were nodeMCU which has builtin button on GPIO0 which is ACTIVE_LOW
and STM32NUCLEO-F429ZI whuch has a user button that is ACTIVE_HIGH

You will probably need to adjust these values for you own device
*/
#ifdef ARDUINO_ARCH_STM32
  #define UB_ACTIVE ACTIVE_HIGH
#else
  #define USER_BTN 0
  #define UB_ACTIVE ACTIVE_LOW
#endif
/*
    ALL GPIO strategies are derived from H4GPIOPin: the following members are available
    inside ALL GPIO pin callbacks, once you have a valid pointer for the pin type using 
    H4GM_PIN( type ) with specific underlying type Raw needs H4GM_PIN(Raw);

    uint8_t         pin=0;                  // GPIO hardware pin number
    uint8_t         gpioType=0;             // INPUT, INPUT_PULLUP, OUTPUT etc
    H4GM_STYLE      style;                  // Strategy: Raw, debounced, Encoder etc
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
            
    Additional members for Encoder:

        int             autoValue // current encoder value

    Additional functions using EncoderAutoPin*

		void setValue(int v);
		void setMin(){ setValue(vMin);	}	
		void setMax(){ setValue(vMax);	}		
		void setPercent(uint32_t pc){ setValue((vMax-vMin)*pc/100); }		
		void setCenter(){ setValue((vMin+vMax)/2); }

        Think of this as an "absolute" encoder or Encoder strategy on steroids. At creation time, you 
        provide a minimum and a maximum value and an increment amount. By default the EncoderAuto will be
        set to a starting value of the mid-point between min and max, but you can set it anywhere you like. 

        Each time it is clicked in the clockwise diretion it wil add the increment to the current value, 
        and subtract it when turned clockwise. For example if you choose min = 0, max = 100 and increment = 2 
        then the following will happen:

        - You will receeive a startup pseudo-event with additional field `autoValue` set to 50.
        - After one clockwise click you get a callback with  `autoValue`  set to 52
        - While you turn it back another 3 anti-clockwise clicks, you get a 3 more callbacks callback with  `autoValue`  set to 50, 48, and 46
        - Once you reach 0, no amount of anti-clockwise turns will change the value below 0
        - Once you reach 100, no amount of clockwise turns will change the value above 100

        Min and max may be negative: you could have Min=-273 and Max=-1

        It also has a variant that takes the name of an `int` variable instead of a callback: the variable
        is set to the current value automatically when the encoder is turned.
       
*/
#define ENCODER_A D7
#define ENCODER_B D6

void h4setup() { // H4 constructor starts Serial
    Serial.println("H4P_GPIOManager EncodeAuto Example v"H4P_VERSION);
    Serial.print("GPIO ");Serial.print(USER_BTN);Serial.print(" ACTIVE ");Serial.println(UB_ACTIVE ? "HIGH":"LOW");
//
//  min=0, max=100, -1 means set inital value to default (50 in this case), increment by 2 per click
//
    h4gm.EncoderAuto(ENCODER_A,ENCODER_B,INPUT,UB_ACTIVE,0,100,-1,2,[](H4GPIOPin* ptr){
        H4GM_PIN(EncoderAuto); // Create the correct pointer type in 'pin'
        Serial.print("ENCODER AUTO value ");Serial.println(pin->autoValue);
    });
}
