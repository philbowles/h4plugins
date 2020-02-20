![H4P Flyer](/assets/GPIOLogo.jpg) 

# GPIO Manager (short name="gpio")

## Adds sophisticated GPIO management to H4 Universal Scheduler/Timer.

![H4P Flyer](/assets/GPIO.jpg)

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---

# What does it do?

H4 GPIO Manager provides easy-to-use "strategies" for many of the common behaviours of buttons, switches and sensors. It does all the "heavy lifting" in one line of code and simply calls you when an interesting event occurs on the GPIO pin.

Unfortunately, "interesting" in the real world often translates to "difficult" because even  the most expensive of switches will have "switch bounce". Many devices you might want to hook up are bouncy, noisy, or complicated. Some e.g. rotary encoders are usually all three at once...

H4 GPIO Manager takes away the pain and allows you to get on quickly with the *really* intersting stuff which is what you want your device to *do* without having to worry about all the low-level nastiness.

Perhaps the best part though is that once you have your code working on one type of MCU it should be a "straight lift" to a different type of device where it should "just work".

---

# Usage

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS
H4P_GPIOManager h4gm;
```

## Dependencies

none

## Commands Added

h4/show/pins

## Unloadable

YES: No GPIO activity is futher handled. This cannot be undone.

---

## Introduction

Let's face it: there are only so many things you can do with a GPIO pin! Yes, there are thousands of different type of sensor out there but each and every one of them can only set a GPIO pin HIGH (usually 5v or 3.3v) or LOW (usually 0v). What differentiates them and makes them clever is *the way* they do that.

GPIOManager currently provides behaviours for:

* AnalogThreshold - send 0 or 1 depending on raw analog value > or < user-defined limit
* Circular - Counts 1 - N, 1 - N etc
* Debounced - Eliminates switch bounce
* Encoder - Standard quadrature rotary encoder
* EncoderAuto - Encoder that maintains / adjusts internal absolute value
* Filtered - Filters out 1 or 0 states
* Latching - Turns "tact" button into latching switch
* Multistage - Calls different functions the longer a button is held ON
* Output
* Polled - Called periodically to report changed digital state or analog value 
* Raw - Pure 1 / 0 states
* Repeating - Calls repeatedly while held ON
* Retriggering - Resets after timeout, additonal triggers restart timer
* Sequenced - Increments counter for each ON/OFF press
* Timed - Reports how long button held ON

You should also read [Things vs Switches](things.md) which explains how some of the above strategies can be autmatically tied/bound/linked to predefined output actions. In it you will find how to use these variants of the above, each of which send an ON or OFF command to a linked output handler

* AnalogThresholdThing - binds a AnalogThreshold input to a BinarySwitch or UPNPSwitch output
* DebouncedThing - binds a Debounced input to a BinarySwitch or UPNPSwitch output
* EncoderThing - binds a Encoder input to a BinarySwitch or UPNPSwitch output
* LatchingThing - binds a Latching input to a BinarySwitch or UPNPSwitch output
* PolledThing - binds a Polled input to a BinarySwitch or UPNPSwitch output 
* RawThing - binds a Raw input to a BinarySwitch or UPNPSwitch output
* RetriggeringThing - binds a Retriggering input to a BinarySwitch or UPNPSwitch output

Before we dive into the strategies, let's deal with the sometimes confusing concept of "Active High" and "Active Low" (experts can skip the next section)

## Logical vs Physical

Hopefully, you will be familiar with Arduino's `digitalRead` and `digitalWrite` as the "standard" way of dealing with GPIO. To use them successfully you need to understand the concept of "Active High" and "Active Low".

Most of us think of a sensor or LED for example as being "ON" when there is a voltage on the GPIO pin after we set it high: `digitalWrite(MY_PIN,HIGH);` and this is what is known as "Active High". We say that the device is "active" (or "ON") when the GPIO pin is HIGH, which is all quite easy to understand.

But many devices work "the other way round": they are "ON" when the GPIO pin is LOW and OFF when it is high and that can get confusing. Most ESP8266 boards have the built-in LED wired this way. Many that have a user button (e.g. nodeMCU) also have the button wired as "Active Low".

To make life more fun, different boards do it different ways - some even have it different ways on different boards in the same range (e.g. STM32-NUCLEO).

Dealing with GPIO this way is the *physical* way of dong it, we have to write the actual 1 or 0 to the GPIO pin.

GPIO manager works differently. When you attach a strategy to a pin, you tell it whether the pin is active high or low. From that point onwards you set it ON or OFF to correspond with the real-world concept of whether it is actually "doing its thing" or not.

GPIO manager provides `logicalWrite` to make your life easy and is best shown in an example:

```cpp
h4gm.Output(LED_BUILTIN,ACTIVE_LOW,OFF);
...
h4gm.logicalWrite(LED_BUILTIN,ON); // does exactly what it says on the tin

```

## The benefits of the logical approach

The code above sets the `pinMode` for you and also sets the initial state of the pin to its logical state (in this case, OFF).

Already you have saved a little code, even if its only one line but the benefits continue:

* If you move the code to a different board whose LED works the opposite way, all you change is `ACTIVE_LOW` to `ACTIVE_HIGH` and all the rest of your code will work without needing to find every `digitalWrite` and change HIGH to LOW.
* It's mentally easier: `logicalWrite(pinN,ON)` always turns the LED ON, whether its active low or high. Get it right once at the start and never worry about it again.
* IF you get it wrong first time, one tiny change is all it takes to fix everything else.

---

# Strategies / behaviours

## Overview

In your sketch, you attach a named strategy to a GPIO pin. Almost* all such calls have a final parameter which is a "callback" function which you provide. It is given a pointer which allows you to get information from the pin and / or control it. Any time the pin changes its state your callback is notified. What constitues "state" has different meanings for the different strategies. For example as "Latching" GPIO changes its logical state only after every two physical changes (1x ON, 1xOFF) and an "Encoder" only after a whole series of quadrature-encoded physical changes across *two* pins.

(*Encoder and EncoderAuto both have variants where the final parameter is the name of a variable whose value is automatically updated as the encoder rotates.)

Don't worry if this is new to you: there are dozens of examples you can cut and paste which show how easy it is.

All strategies continually collect the following common information on each GPIO pin. Some provide further information or functions to manage their specifc needs, these are detailed in the relevant section.

### Common data fields:

All times are in microseconds (uS) = 1/1000000 sec

```cpp
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

```

The most important of these is `state`. It is the current **logical** state, i.e. 1 = ON and 0 = OFF. If you use GPIO manager correctly you never need to know, examine, read or write the physical state of the pin. *Always* use `logicalRead` and/or `logicalWrite`

These are included for completeness: most of the time the strategy handles these values automatically - that's its purpose in life - but for more advanced users, you have full access. Read them if you want, write to them at your peril - it will almost certainly break the stategy if you do.

### Common Initial "fake" Events

The fundamental programming model of GPIO manager is that you get called only when a GPIO pin *changes*. But at startup, - by defintion - nothing can have changed. However you probably need to do some initial setup depending on the boot-time state of the pin.

Most* strategies call you back at boot time so that you are aware of that intitial state. Be aware though, that this is not specifically an "Event" as such because - as already mentioned - nothing has changed. For example, it is quite possible for a PIR sensor to be already "triggered" when your MCU powers up. You need to set immediately whatever your MCU does in the "triggered" state (e.g. turn on security light) rather than wait for the "real" event OFF timeout to occur "out of nowhere" a few seconds later.

(*Some strategies have no concept of "inital state" for example a rotary encoder that is only "interesting" while it is being turned. Since it hasn't moved yet, there is nothing to do: It isn't signalling "clockwise" or "anti-clockwise" which are its only two meaningful values, so what "state" can it be said to be in?)

Your code may need to differentiate between this first (and only) "fake" event and all subsequent "real" ones. This is made very easy as in all cases the `nEvents` field will be zero at boot time. In *most* cases, you shouldn't need to do anything different from "the real thing".

### Accessing the pin object

All strategies return a pointer to the pin. Most of the time you don't need this, as it is the same as the pointer passed to the callback function.

It is useful however if a pin needs to "talk" to another: save the second pin's pointer when creating it and use this in the first pin's callback to get the state of the second.

Every different pin strategy returns a different pin pointer: Raw returns a RawPin*, Retriggering returns a RetriggeringPin* and so on. To make life easier in your callback and get the right type of pointer, use the `H4GM_PIN` macro as the very first line of the callback routine.

Leave the callback function parameter as the default of "ptr" and the `H4GM_PIN` macro will convert it into "pin" which is always the correct type of pointer, for example:

```cpp
  h4gm.Circular(USER_BTN,INPUT,UB_ACTIVE,U_DBTIME_MS,U_N_STAGES,[](H4GPIOPin* ptr){
    H4GM_PIN(Circular); // Create the correct pointer type in 'pin'
    Serial.print("Stage ");Serial.println(pin->stage);
   });
```

## Specific Strategies

### **Raw**

Is more like a *non*-strategy: It passes every single change to the callback. It is a convenient base for your own scenarios (if any) not covered by GPIO manager. The benefit is you automatically get uS time of event, difference since previous (delta) etc and you can "throttle" the pin. This is explained in more detail later, but means you can set a maximum rate (cps) of events to handle per second.
[Example Code](../examples/H4GM_Raw/H4GM_Raw.ino)

---

### **Filtered**

Works like **Debounced** but only sends all ONs or all OFFs , i.e. it filters out the ones you don't want.
[Example Code](../examples/H4GM_Filtered/H4GM_Filtered.ino)

---

### **Debounced**

Probably one of the most useful: if you don't know what switch bounce is then you **need** this and you also need to read and understand [this link](http://www.ganssle.com/debouncing.htm). If you *do* know what switch bounce is, then you also need this, as it "just works". The callback receives only "clean" ON/OFF changes once you set the appropriate *millisecond* (mS) 1/1000 sec value for the particular switch / button.
[Example Code](../examples/H4GM_Debounced/H4GM_Debounced.ino)

---

### **Latching**

Based on **Debounced**, it retains its state between press/release pairs. Press and release once and it is "latched" ON. It will stay ON until you press and release again to un-latch it.

#### Additional fields for Latching

`uint32_t        latched; // alternately ON or OFF`
[Example Code](../examples/H4GM_Latching/H4GM_Latching.ino
)

---

### **Sequenced**

Based on **Debounced**, it only calls you after a *pair* of ON/OFF changes, i.e. a single press/relase of a button. After each such press, the additional field `stage` contains a count of the numer of ON/OFF pairs.

#### Additional fields for Sequenced

`uint32_t        stage; // Ever-increasing number of times ON/OFF pair occurred`
[Example Code](../examples/H4GM_Sequenced/H4GM_Sequenced.ino)

---

### **Circular**

Based on **Sequenced**, you provide a maximum number of stages and then the value "wraps round". For example if you set the maximum to 3, and press the button 7 times the additional field `stage` will contain 1 then 2 then 3 then 1,2,3 again then 1... and so on. This can be useful for making a selection from 1 of N items using only a single button.

#### Additional fields for Circular

`uint32_t        stage; // stage of cycle`
[Example Code](../examples/H4GM_Circular/H4GM_Circular.ino)

---

### **Timed**

Based on **Debounced**, when the state changes back to OFF the additional field `held` will contain the number of microseconds the button was held ON.

#### Additional fields for Timed

`uint32_t        held; // Number of microseconds GPIO was held ON`
[Example Code](../examples/H4GM_Timed/H4GM_Timed.ino)

---

### **Repeating**

Based on **Timed**, it will continue to call your function while ever the GPIO is held ON. You choose how often you get called (the *frequency*) and when the state changes back to OFF the additional field `held` will contain the number of microseconds the button was held down.
[Example Code](../examples/H4GM_Repeating/H4GM_Repeating.ino)

---

### **Retriggering**

Think of a PIR movement sensor: On first detection it goes ON and stays on for a chosen period (the "timeout"). Any *new* movement before the timout period expires will cause it to start all over again, staying ON and re-starting the clock or "re-triggering the timeout".

The Retriggering strategy is identical. You get a callback when the pin changes to ON and then again to OFF following `timeout` milliseconds after the final physical retriggering event. 
[Example Code](../examples/H4GM_Retriggering/H4GM_Retriggering.ino)

---

### **Polled**

**Polled** has its state checked periodically, usually a large value, e.g. minutes. It is used when you do *not* want to react quickly. A good example is a light sensor at dusk: you want lights to come on when it is "dark". As the light fades the sensor will "flutter" rapidly for quite a long time before settling to its "dark" value. Only *then* do you want to switch the lights.

Checking such a sensor every 5 minutes is usually more than enough. The important thing to note is that *you do not get called every N minutes* - you get called on an N-minute multiple only if the state has changed from the previous reading N minutes ago.

This strategy can also do an `analogRead` if required: `state` will hold the raw analog value;
[Example Code](../examples/H4GM_Polled/H4GM_Polled.ino)

---

### **AnalogThreshold**

**AnalogThreshold** is a specialised version of **Polled**. As with **Polled** it has its state checked periodically but it then converts the raw analog value to a 0 or a 1 and compares it with a threshhold value. It will only execute the callback function if the > or < test against the threshold succeeds.

#### Additional fields for AnalogThreshold

`uint32_t        limit; // The threshold value`

The constructor has a `compare` parameter which can be either `H4GM_LESS` or `H4GM_GREATER`

* If the raw value of the encoder is 77 and you specify a limit of 100 and use `H4GM_LESS` then the callback will receive 1 as 77 is less than 100
* If the raw value of the encoder is 77 and you specify a limit of 100 and use `H4GM_GREATER` then the callback will receive 0 as 77 is NOT greater than 100
* If the raw value of the encoder is 100 and you specify a limit of 512 and use `H4GM_LESS` then the callback will receive 1 as 100 is less than 512
* If the raw value of the encoder is 100 and you specify a limit of 512 and use `H4GM_GREATER` then the callback will receive 0 as 100 is NOT greater than 512

[Example Code](../examples/H4GM_AnalogThreshold/H4GM_AnalogThreshold.ino)

---

### **Encoder**

Manages a rotary encoder, including all necessary debouncing and decoding. You get called with additional field `encoderValue` of -1 for every anti-clockwise click or +1 for clockwise. It's that simple. Obviously it requires two physical pins but one single call does everything for you.

It also has a variant that takes the name of an `int` variable instead of a callback: the variable is incremented or decremented automatically when the encoder is turned.

#### Additional fields for Encoder

`int        encoderValue; // +1 clockwise click. -1 anti-clockwise`

[Example Code](../examples/H4GM_Encoder/H4GM_Encoder.ino)

---

### **EncoderAuto**

Think of this as an "absolute" encoder or **Encoder** on steroids. At creation time, you provide a minimum and a maximum value and an increment amount. By default the **EncoderAuto** will be set to a starting value of the mid-point between min and max, but you can set it anywhere you like within the valid range of min ans max. 

Each time it is clicked in the clockwise direction it wil add the increment to the current value, and subtract it when turned clockwise. For example if you choose min = 0, max = 100 and increment = 2 then the following will happen:

 - You will receeive a startup pseudo-event with additional field `autoValue` set to 50.
 - After one clockwise click you get a callback with  `autoValue`  set to 52
 - While you turn it back another 3 anti-clockwise clicks, you get a 3 more callbacks with  `autoValue`  set to 50, 48, and 46
 - Once you reach 0, no amount of anti-clockwise turns will change the value below 0
 - Once you reach 100, no amount of clockwise turns will change the value above 100

Min and max may be negative: you could have Min=-273 and Max=-1

It also has a variant that takes the name of an `int` variable instead of a callback: the variable is set to the current value automatically when the encoder is turned.

#### Additional fields for EncoderAuto

`int       autoValue; // current value of encoder`

#### Additional methods for EncoderAuto

**EncoderAuto** Also provides the following methods.

```cpp
setValue // manually set value;
setMin  // change minimum value
setMax // change maximum value
setPercent // manually set value to % of max - min
setCenter // manully set the value to the mid-point (max - min / 2)

```

[Example Code](../examples/H4GM_EncoderAuto/H4GM_EncoderAuto.ino)

---

### **Multistage**

**Multistage** has been left till last because it can appear quite complex. The easiest way to describe it is to think of devices that behave differently the longer you hold the button. e.g. "pairing" mode on many WiFi devices if you hold the on/off switch for more than - say - five seconds.

Often these devices will show a visual indication as they change modes for example turning an LED to red when pairing. To replicate this behaviour we need something like a **Repeating** GPIO but one that calls a different function as the time held down gets longer so that the function can change LED colour etc. It then needs to perform a different action once the button is released, depending on how long it was held down for.

Enter **Multistage** which is designed for exactly that purpose.

It requires a table of times and functions as well as the normal callback. The normal callback happens every time a threshold is passed while ON is held, but the OFF event is sent to the function in the table that matches the current time threshold, for example:

```cpp
  H4GM_STAGE_MAP stageMap={
    {0,switchOnOff}, // anything up to 2 seconds call switchOnOff
    {2000,h4reboot}, // anything between 2 and 5 seconds call reboot
    {5000,factoryReset} // anything over 5 seconds call factoryReset
  };
```

You can just leave it at that and have the "short" press call `switchOnOff` when released; a "medium" press call `reboot` when released and a "long" press call `factoryReset` on release, but...

To change the speed of a flashing LED as each stage is passed, have your "normal" callback function do something like this:

```cpp
void progress(H4GPIOPin* p){
   H4GM_PIN(Multistage); // Create the correct pointer type in 'pin'
   Serial.print("progress ");Serial.print(pin->stage);Serial.print(" held ");Serial.println(pin->held);
   // case stage = 0 is up to 2 seconds, do nothing 
   if(case){
    switch(pin->stage){
        case 1:
            h4fc.flashLED(250);
            break;
        case 2:
            h4fc.flashLED(50);
            break;
        default:
            pin->dump();
        }
   }
}

```

For simplicity the code calls another plugin [H4P_FlasherController](../h4fc) to seamlessly flash the LED and change its rate. The table can have as many entries as you like, the actual call will be:

```cpp
// 15 is mS debounce time
  h4gm.Multistage(USER_BTN,INPUT,ACTIVE_LOW,15,stageMap,progress);

```

#### Additional fields for Multistage

`int       stage; // current table entry not exceeding held time`

`uint32_t        held; // Number of microseconds GPIO was held ON`

[Example Code](../examples/H4GM_Multistage/H4GM_Multistage.ino)

---

# Pin "Throttling"

Some sensors can change the physical state of the pin too fast for a low-powered MCU to keep up, especially if it has a lot of work to do on each change. Left unmanaged this can (and usually *does*) result in rapid depletion of resources leading to a crash and/or reboot. GPIO manager automatically calculates the rate of changes per second (cps) for every pin and allows you to set a maximum permitted value. Any changes exceeding the cps value in each 1-second time slice are simply not passed through to your callback, so it must be written to cope with "missing" events. For example if it depends on have an ON/OFF pair in the data stream, it will break if throttled.

It is important to understand that this is a drastic response to a problem that should really have a "proper" solution, such as changing your code or using a faster MCU, but it may buy you enough time to diagnose complex issues.

An example is a sound sensor: these can often cause tens of thousands of changes per second in a short burst on detecting - say - a hand clap. If all you care about is that a big burst of data means "some noise happened", then the exact number of changes is irrelevant as long as it is *large*. Handling every single event wastes MCU time and slows other processes, so throttling such a pin to a very low value say 10 per second instead of 10,000 does the same job much more efficiently. You may have to experiment to find the right balance.

---

## API

```cpp
// common parameters:
// p = GPIO pin number
// mode = INPUT, INPUT_PULLP etc as in Arduino pinMode
// sense = either ACTIVE_HIGH or ACTIVE_LOW
// dbTimesMS = milliseconds debounce time
// callback = either a lambda or "normal" function name taking one parameter, an H4GPIOPin*
// This need to be converted to the correct pin type using H4GM_PIN( pin type ) macro before use
// for other parameters not named, see individual strategies above
//
//  General-purpose
//
// returns 32bits not 8 as it can also do analogRead and state can hold raw analog value or digital 1/0
uint32_t        logicalRead(uint8_t p);
void            logicalWrite(uint8_t p,uint8_t l);
void            throttle(uint8_t p,uint8_t v); // limit rate of throughput per second
void            toggle(uint8_t p); // reverse current logical state
//
//      Strategies
//
AnalogThresholdPin* AnalogThreshold(uint8_t p,uint32_t freq,uint32_t threshold,H4GM_COMPARE compare,H4GM_FN_EVENT callback);//
AnalogThresholdPin* AnalogThresholdThing(uint8_t p,uint32_t freq,uint32_t threshold,H4GM_COMPARE compare,H4P_BinaryThing* btp);//
CircularPin*        Circular(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,uint32_t nStages,H4GM_FN_EVENT callback);//
DebouncedPin*       Debounced(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4GM_FN_EVENT callback);//
DebouncedPin*       DebouncedThing(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4P_BinaryThing* btp);//
EncoderPin*         Encoder(uint8_t pA,uint8_t pB,uint8_t mode,H4GM_SENSE sense,H4GM_FN_EVENT);
EncoderPin*         Encoder(uint8_t pA,uint8_t pB,uint8_t mode,H4GM_SENSE sense,int&);
EncoderPin*         EncoderThing(uint8_t pA,uint8_t pB,uint8_t mode,H4GM_SENSE sense,H4P_BinaryThing* btp);
EncoderAutoPin*     EncoderAuto(uint8_t pA,uint8_t pB,uint8_t mode,H4GM_SENSE sense,int vMin,int vMax,int vSet,uint32_t vIncr,H4GM_FN_EVENT);
EncoderAutoPin*     EncoderAuto(uint8_t pA,uint8_t pB,uint8_t mode,H4GM_SENSE sense,int vMin,int vMax,int vSet,uint32_t vIncr,int&);
FilteredPin*        Filtered(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint8_t filter,H4GM_FN_EVENT callback);//
LatchingPin*        Latching(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4GM_FN_EVENT callback);//
LatchingPin*        LatchingThing(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4P_BinaryThing* btp);//
MultistagePin*      Multistage(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4GM_STAGE_MAP stageMap,H4GM_FN_EVENT callback);//
OutputPin*          Output(uint8_t p,H4GM_SENSE sense,uint8_t initial,H4GM_FN_EVENT callback=nullptr);// FIX ptr type
PolledPin*          Polled(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t frequency,uint32_t isAnalog,H4GM_FN_EVENT callback);//
PolledPin*          PolledThing(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t frequency,uint32_t isAnalog,H4P_BinaryThing* btp);//
RawPin*             Raw(uint8_t p,uint8_t mode,H4GM_SENSE sense,H4GM_FN_EVENT callback);//
RawPin*             RawThing(uint8_t p,uint8_t mode,H4GM_SENSE sense,H4P_BinaryThing*);//
RepeatingPin*       Repeating(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,uint32_t frequency,H4GM_FN_EVENT callback);//
RetriggeringPin*    Retriggering(uint8_t _p, uint8_t _mode,H4GM_SENSE sense,uint32_t timeout, H4GM_FN_EVENT _callback);
RetriggeringPin*    RetriggeringThing(uint8_t _p, uint8_t _mode,H4GM_SENSE sense,uint32_t timeout,H4P_BinaryThing* btp);
SequencedPin*       Sequenced(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4GM_FN_EVENT callback); //
TimedPin*           Timed(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4GM_FN_EVENT callback); //
//
```

---

(c) 2020 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Blog](https://8266iot.blogspot.com)
* [Facebook Esparto Support / Discussion](https://www.facebook.com/groups/esparto8266/)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook IOT with ESP8266 (moderator)}](https://www.facebook.com/groups/1591467384241011/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)
