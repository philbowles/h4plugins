![H4P Flyer](/assets/GPIOLogo.jpg) 

# GPIO Handling

Essential background information before using any of the following. You should also read [Sources vs Switches](things.md) which explains how some of the below can be automatically tied/bound/linked to the default ouptput device `h4onof`, if present.

* `AnalogAverage`: send average of N analog samples
* `AnalogThreshold`: send 0 or 1 depending on raw analog value > or < user-defined limit
* `AnalogThresholdSource`: feeds result direct to default device (`h4onof`)
* `Circular`: Counts 1: N, 1: N etc
* `Debounced`: Eliminates switch bounce
* `Encoder`: Standard quadrature rotary encoder
* `EncoderSource`: feeds result direct to default device (`h4onof`)
* `EncoderAuto`: Encoder that maintains / adjusts internal absolute value
* `Filtered`: Filters out 1 or 0 states
* `Latching`: Turns "tact" button into latching switch
* `LatchingSource`: feeds result direct to default device (`h4onof`)
* `Multistage`: Calls different functions the longer a button is held ON
* `Output`
* `Polled`: Called periodically to report changed digital state or analog value 
* `PolledSource`: feeds result direct to default device (`h4onof`)
* `Raw`: Pure 1 / 0 states
* `RawSource`: feeds result direct to default device (`h4onof`)
* `Repeating`: Calls repeatedly while held ON
* `Retriggering`: Resets after timeout, additonal triggers restart timer
* `RetriggeringSource`: feeds result direct to default device (`h4onof`)
* `Sequenced`: Increments counter for each ON/OFF press
* `Timed`: Reports how long button held ON

---

# Contents

* [Introduction](#introduction)
* [Logical vs Physical](#logical-vs-physical)
* [Strategies](#strategies)
* [Full API H4P_GPIOManager](h4gm.md)

---

# Introduction

Let's face it: there are only so many things you can do with a GPIO pin. Yes, there are thousands of different type of sensor out there but each and every one of them can only set a GPIO pin HIGH (usually 5v or 3.3v) or LOW (usually 0v) or put an analog voltage on it. What differentiates them and makes them clever is *the way* they do that.

Unfortunately, "clever" in the real world often translates to "difficult" because even  the most expensive of switches will have "switch bounce". If you don't know what that is, here's an [excellent article](http://www.ganssle.com/debouncing.htm) explaining it all and how to deal with it. Many devices you might want to hook up are bouncy, noisy, or complicated. Some e.g. rotary encoders are usually all three at once...

[H4P_GPIOManager](h4gm.md) provides easy-to-use "strategies" for many of the common behaviours of buttons, switches and sensors. It does all the "heavy lifting", usually in a single line of code and simply calls you when an interesting event occurs on the GPIO pin.

---

# Logical vs Physical

Hopefully, you will already be familiar with Arduino's `digitalRead` and `digitalWrite` as the "standard" way of dealing with GPIO pins. To use them successfully you need to understand the concept of "Active High" and "Active Low".

Most of us think of a sensor or LED for example as being "ON" when there is a voltage on the GPIO pin after we set it high: `digitalWrite(MY_PIN,HIGH);` and this is what is known as "Active High". We say that the device is "active" (or "ON") when the GPIO pin is HIGH, which is all quite easy to understand.

But many devices work "the other way round": they are "ON" when the GPIO pin is LOW and OFF when it is high and that can get confusing. Most ESP8266 boards have the built-in LED wired this way. Many that have a user button (e.g. nodeMCU) also have the button wired as "Active Low".

Dealing with GPIO this way is the *physical* way of doing it, we have to write the actual 1 or 0 to the GPIO pin.

[H4P_GPIOManager](h4gm.md)works differently. When you attach a strategy to a pin, you tell it whether the pin is active high or low. From that point onwards you set it ON or OFF to correspond with the real-world concept of whether it is actually "doing its thing" or not.

This is best shown in an example, using [H4P_GPIOManager](h4gm.md)'s `logicalWrite` function:

```cpp
// We declare out pin is "Active Low" (ON when pin=0) and set it intially OFF
h4gm.Output(LED_BUILTIN,ACTIVE_LOW,OFF);
...
h4gm.logicalWrite(LED_BUILTIN,ON); // does exactly what it says on the tin

```

## Benefits of the logical approach

The code above sets the `pinMode` for you and also sets the initial state of the pin to its logical state (in this case, OFF). Already you have saved a little code, even if its only one line but the benefits continue:

* If you move the code to a different board whose LED works the opposite way, all you change is `ACTIVE_LOW` to `ACTIVE_HIGH` and all the rest of your code will work without needing to find every `digitalWrite` and change HIGH to LOW.
  
* It's mentally easier: `logicalWrite(pin,ON)` always turns the LED ON, whether its active low or high. Get it right once at the start and never worry about it again.
  
* *IF* you get it wrong first time, one tiny change is all it takes to fix everything else.

---

# Strategies

## Overview

In your sketch, you attach a strategy to a GPIO pin. Almost all such calls have a final parameter which is a "callback" function which you provide. It is given a C/C++ pointer which allows you to get information from the pin and / or control it. Any time the pin changes its *logical* state your callback is notified. What constitues "*logical* state" has different meanings for the different strategies. Some examples might help:

* A `Debounced` GPIO may change its *physical* state 4, 6, 8 or 10 times before it "settles" into its new *logical* state. This is what switch bounce *is*. Your callback will not happen until the bouncing has settled, and the *logical* `state` at that point will be the new steady value.

* Even *after* being debounced, a `Latching` GPIO only changes its logical state after every alternating ON/OFF pair. Press release once, logical state is ON. Press / release again, logical state is now OFF.

* The physical states of the *two* pins of an `Encoder` will change *at least* 4x between them per "click" and possibly *dozens* if it is "bouncy". Its *logical* state changes ony once per click. If it was turned clockwise, the state will be 1, but when truned *anti*clockwise its logical state is -1, yes negative.

* The logical state of an Analog GPIO is its raw ADC value

As you can see, the logical state of a GPIO is more than just 1 or 0 and more closely matches what the pin is being used for.

## Common data fields:

All strategies continually collect the following common information on each GPIO pin. Some provide further information or functions to manage their specifc needs, these are detailed in the relevant section.

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
    uint32_t        rate=0;                 // instantaneous rate transitions / sec
    uint32_t        Rpeak=0;                // peak rate "
    uint32_t        cps=0;                  // sigma transitions / sec (used internally, frequently re-set)
    uint32_t        cMax=UINT_MAX;          // max permitted cps (see "throttling");
    uint32_t        nEvents=UINT_MAX;       // sigma "Events" (meaning depends on inheriting strategy)
```

The most important of these is `state`. It is the current **logical** state, i.e. 1 = ON and 0 = OFF. If you use GPIO manager correctly you never need to know, examine, read or write the physical state of the pin. *Always* use `logicalRead` and/or `logicalWrite`

These are included for completeness: most of the time the strategy handles these values automatically - that's its purpose in life - but for more advanced users, you have full access. Read them if you want, ***DO NOT WRITE TO THEM!***

## Source variants

Any of the strategies that have a `...Source` variant will pass the logical state change directly to the default output device `h4onf` if you have one.

Imagine you have an external light set up as [H4P_BinarySwitch](swings.md). You can turn it on / off using a manual switch or any of the other H4Plugins methods, MQTT, HTTP Rest, Alexa voice etc. Now imagine you have a daylight sensor connected as a `PolledSource` which reads the state of the sensor every 5 minutes.

Providing everything is wired correctly, the light will come on automatically as soon as it gets dark. You could also have a PIR sensor configured as a `RetriggeringSource` which will automatically turn the light on if there is any movement.

For more information, see [Sources vs Switches](things.md)

## Common Initial "fake" Events

The fundamental programming model of GPIO manager is that you get called only when a GPIO pin's logical state *changes*. But at startup, - by defintion - nothing can have changed, or everything has, depending on your phiolosophy. However you may need to do some initial setup depending on the boot-time state of the pin.

Most strategies call you back at boot time so that you are aware of that intitial state. Be aware though, that this is not specifically an "Event" as such because - as already mentioned - *nothing has changed*. For example, it is quite possible for a PIR sensor to be already "triggered" when your MCU powers up. You need to set immediately whatever your MCU does in the "triggered" state (e.g. turn on security light) rather than wait for the "real" event OFF timeout to occur "out of nowhere" a few seconds later.

Some strategies have no concept of "inital state" for example a rotary encoder that is only "interesting" *while it is being turned*. Since it hasn't moved yet, there is nothing to do: It isn't signalling "clockwise" or "anti-clockwise" which are its only two meaningful values, so what "state" can it be said to be in?

Your code may need to differentiate between this first (and only) "fake" event and all subsequent "real" ones. This is made very easy as in all cases the `nEvents` field will be zero at boot time. In *most* cases, you shouldn't need to do anything different from "the real thing" that you would normally do.

## Accessing the pin object in code

All strategies return a pointer to the pin. Most of the time you don't need this, as it is the same as the pointer passed to your callback function. It can be useful however if a pin needs to "talk" to another: save the second pin's pointer when creating it and use this in the first pin's callback to get the state of the second, or control it in some way.

Every different pin strategy returns a different pin pointer which matches the pin type: `Raw` returns a `RawPin*`, `Retriggering` returns a `RetriggeringPin*` etc. To make life easier in your callback and get the right type of pointer, use the `H4GM_PIN` macro as the very first line of the callback routine.

Leave the callback function parameter as the default of "ptr" and the `H4GM_PIN` macro will convert it into "pin" which is always the correct type of pointer, for example:

```cpp
  h4gm.Circular(USER_BTN,INPUT,UB_ACTIVE,U_DBTIME_MS,U_N_STAGES,[](H4GPIOPin* ptr){
    H4GM_PIN(Circular); // Create the correct pointer type in 'pin'
    Serial.print("Stage ");Serial.println(pin->stage); // pin is now CircularPin*
   });
```

Get into the habit of doing this: if you get the pin pointer type wrong, or leave it as the default `H4GPIOPin* ptr`, bad things may happen with some GPIO types. Be safe, don't use `ptr` directly and always call `H4GM_PIN( < whatever> )` before using pin as a pointer.

## Pin "Throttling"

Some sensors can change the physical state of the pin too fast for a low-powered MCU to keep up, especially if it has a lot of work to do on each change. Left unmanaged this can (and usually *does*) result in rapid depletion of resources leading to a crash and/or reboot. GPIO manager automatically calculates the rate of changes per second (cps) for every pin and allows you to set a maximum permitted value. Any changes exceeding the cps value in each 1-second time slice are simply not passed through to your callback, so it must be written to cope with "missing" events. For example if it depends on have an ON/OFF pair in the data stream, it may break if throttled and the "matching" OFF event is throttled away.

It is important to understand that this is a drastic response to a problem that should really have a "proper" solution, such as changing your code or using a faster MCU, but it may buy you enough time to diagnose complex issues.

An example is a sound sensor: these can often cause tens of thousands of changes per second in a short burst on detecting - say - a hand clap. If all you care about is that a big burst of data means "some noise happened", then the exact number of changes is irrelevant as long as it is *large*. Handling every single event wastes MCU time and slows other processes, so throttling such a pin to a very low value say 10 per second instead of 10,000 does the same job much more efficiently and prevents problems. You may have to experiment to find the right balance.

---

# Individual strategies

These are shown in "hierarchy" order, rather than alphabetically as many expand on functions of previous strategies. It helps to understand how each strategy works if you read through them in "the right order". The diagram is out-of-date, but will at least give you an idea of how they all "hang together"

![gpio](../assets/GPIO.jpg)

---

## **Raw**

Is more like a *non*-strategy: It passes every single change to the callback. It is a convenient base for your own scenarios (if any) not covered by GPIO manager. The benefit is you automatically get uS time of every event, the difference since previous (delta) etc and you can "throttle" the pin. As explained [above](#pin-throttling) it means you can set a maximum rate (cps) of events to handle per second.

[Example Code](../examples/GPIO/H4GM_Raw/H4GM_Raw.ino)

---

## **Filtered**

Works like **Debounced** but sends only all ONs or all OFFs , i.e. it filters out the ones you don't want. It's a bit like using an interrupt and watching for a leading edge or  a trailing edge.

[Example Code](../examples/GPIO/H4GM_Filtered/H4GM_Filtered.ino)

---

## **Debounced**

Probably one of the most useful: if you don't know what switch bounce is then you **need** this and you also need to read and understand [what bounce is](http://www.ganssle.com/debouncing.htm). If you *do* know what switch bounce is, then you already know *why* you need this, as it "just works". The callback receives only "clean" ON/OFF changes once you set the appropriate *millisecond* value for the particular switch / button.

[Example Code](../examples/GPIO/H4GM_Debounced/H4GM_Debounced.ino)

---

## **Latching**

Based on **Debounced**, it retains its state between press/release pairs. Press and release once and it is "latched" ON. It will stay ON until you press and release again to un-latch it back to OFF

### Additional fields for Latching

`uint32_t        latched; // alternately ON or OFF`

[Example Code](../examples/GPIO/H4GM_Latching/H4GM_Latching.ino)

---

## **Sequenced**

Based on **Debounced**, it only calls you after a *pair* of ON/OFF changes, i.e. a single press/relase of a button. After each such press, the additional field `stage` contains a count of the numer of ON/OFF pairs. This count will simply keep increasing until it "rolls over" when it passes 2^32 or the device is rebooted.

### Additional fields for Sequenced

`uint32_t        stage; // Ever-increasing number of times ON/OFF pair occurred`

[Example Code](../examples/GPIO/H4GM_Sequenced/H4GM_Sequenced.ino)

---

## **Circular**

Based on **Sequenced**, you provide a maximum number of stages and then the value "wraps round". For example if you set the maximum to 3, and press the button 7 times the additional field `stage` will contain 1 then 2 then 3 then 1,2,3 again then 1... and so on. This can be useful for making a selection from 1 of N items using only a single button.

### Additional fields for Circular

`uint32_t        stage; // stage of cycle`

[Example Code](../examples/GPIO/H4GM_Circular/H4GM_Circular.ino)

---

## **Timed**

Based on **Debounced**, when the state changes back to OFF the additional field `held` will contain the number of *micro*seconds the button was held ON.

### Additional fields for Timed

`uint32_t        held; // Number of microseconds GPIO was held ON`

[Example Code](../examples/GPIO/H4GM_Timed/H4GM_Timed.ino)

---

## **Repeating**

Based on **Timed**, it will continue to call your function while ever the GPIO is held ON. You choose how often you get called (the *frequency*) and when the state changes back to OFF the additional field `held` will contain the number of *micro*seconds the button was held down.

[Example Code](../examples/GPIO/H4GM_Repeating/H4GM_Repeating.ino)

---

## **Retriggering**

Think of a PIR movement sensor: On first detection it goes ON and stays on for a chosen period (the "timeout"). Any *new* movement before the timout period expires will cause it to start all over again, staying ON and re-starting the clock or "re-triggering the timeout".

The Retriggering strategy is identical. You get a callback when the pin changes to ON and then again to OFF following `timeout` milliseconds after the final physical retriggering event.

[Example Code](../examples/GPIO/H4GM_Retriggering/H4GM_Retriggering.ino)

---

## **Polled**

**Polled** has its state checked periodically, usually a large value, e.g. minutes. It is used when you do *not* want to react quickly. A good example is a light sensor at dusk: you want lights to come on when it is "dark". As the light fades the sensor will "flutter" rapidly for quite a long time before settling to its "dark" value. Only *then* do you want to switch the lights.

Checking such a sensor every 5 minutes is usually more than enough. The important thing to note is that *you do not get called every N minutes* - you get called on an N-minute multiple only if the state has changed from the previous reading N minutes ago.

This strategy can also do an `analogRead` if required: `state` will hold the raw analog value;

[Example Code](../examples/GPIO/H4GM_Polled/H4GM_Polled.ino)

---

## **Analog Average**

**AnalogAverage** is a specialised version of **Polled**. As with **Polled** it has its state value sampled periodically. After N samples, it sends the average value.

### Additional fields for AnalogAverage

`uint32_t        nSamples; // The number of samples to average`

[Example Code](../examples/GPIO/H4GM_AnalogAverage/H4GM_AnalogAverage.ino)

---

## **AnalogThreshold**

**AnalogThreshold** is a specialised version of **Polled**. As with **Polled** it has its state checked periodically but it then converts the raw analog value to a 0 or a 1 depending on whether the raw value is higher or lower than some threshhold value. It will only execute the callback function if the > or < test against the threshold succeeds. In effect, this converts an analog pin into a digital pin.

### Additional fields for AnalogThreshold

`uint32_t        limit; // The threshold value`

The constructor has a `compare` parameter which can be either `H4GM_LESS` or `H4GM_GREATER`

* If the raw value of the pin is 77 and you specify a limit of 100 and use `H4GM_LESS` then the callback will receive 1 as 77 is less than 100
* If the raw value of the pin is 77 and you specify a limit of 100 and use `H4GM_GREATER` then the callback will receive 0 as 77 is NOT greater than 100
* If the raw value of the pin is 100 and you specify a limit of 512 and use `H4GM_LESS` then the callback will receive 1 as 100 is less than 512
* If the raw value of the pin is 100 and you specify a limit of 512 and use `H4GM_GREATER` then the callback will receive 0 as 100 is NOT greater than 512

[Example Code](../examples/GPIO/H4GM_AnalogThreshold/H4GM_AnalogThreshold.ino)

---

## **Encoder**

Manages a rotary encoder, including all necessary debouncing and decoding. You get called with additional field `encoderValue` of -1 for every anti-clockwise click or +1 for clockwise. It's that simple. Obviously it requires two physical pins but one single call does everything for you.

It also has a variant that takes the name of an `int` variable instead of a callback: the variable is incremented or decremented automatically when the encoder is turned.

#### Additional fields for Encoder

`int        encoderValue; // +1 clockwise click. -1 anti-clockwise`

[Example Code](../examples/GPIO/H4GM_Encoder/H4GM_Encoder.ino)

---

## **EncoderAuto**

Think of this as an "absolute" encoder or **Encoder** on steroids. At creation time, you provide a minimum and a maximum value and an increment amount. By default the **EncoderAuto** will be set to a starting value of the mid-point between min and max, but you can set it anywhere you like within the valid range of min and max.

Each time it is clicked in the clockwise direction it wil add the increment to the current value, and subtract it when turned clockwise. For example if you choose min = 0, max = 100 and increment = 2 then the following will happen:

* You will receive a startup pseudo-event with additional field `autoValue` set to 50.
* After one clockwise click you get a callback with  `autoValue`  set to 52
* While you turn it back another 3 anti-clockwise clicks, you get a 3 more callbacks with  `autoValue`  set to 50, 48, and 46
* Once you reach 0, no amount of anti-clockwise turns will change the value below 0
* Once you reach 100, no amount of clockwise turns will change the value above 100

Min and max may be negative: you could have Min=-273 and Max=-1

It also has a variant that takes the name of an `int` variable instead of a callback: the variable is set to the current value automatically when the encoder is turned.

### Additional fields for EncoderAuto

`int       autoValue; // current value of encoder`

### Additional methods for EncoderAuto

**EncoderAuto** Also provides the following methods.

```cpp
setValue(uint32_t v); // manually set value: must be min < v < max
setMin() // set position to minimum value: "fully anticlockwise"
setMax() // set position to maximum value: "fully clockwise"
setPercent(uint32_t pc) // manually set value to % of max - min
setCenter() // manully set the value to the mid-point (max - min / 2)
```

[Example Code](../examples/GPIO/H4GM_EncoderAuto/H4GM_EncoderAuto.ino)

---

## **Multistage**

The easiest way to describe **Multistage** is to think of devices that behave differently the longer you hold the button. e.g. "pairing" mode on many WiFi devices if you hold the on/off switch for more than - say - five seconds.

Often these devices will show a visual indication as they change modes for example turning an LED to red when pairing. To replicate this behaviour we need something like a **Repeating** GPIO but one that calls a different function as the time held down gets longer and longer so that the function can change LED colour etc. It then needs to perform a different action once the button is released, depending on how long it was held down for.

Enter **Multistage** which is does exactly that.

It requires a table of times and functions called "stage map". as well as the "normal" callback. 

The "normal" callback beccomes a kind of "progress counter" and gets called every time the next longer threshhold is passed.

```cpp
H4GM_STAGE_MAP stageMap={
    {0,switchOnOff}, // held down for anything up to 2 seconds, call switchOnOff a "short stab"
    {2000,h4reboot}, // anything between 2 and 5 seconds (a "medium press"), call reboot
    {5000,factoryReset} // anything over 5 seconds (a "long press"), call factoryReset
};
```

You can just leave it at that and have the "short" press call `switchOnOff` when released; a "medium" press call `reboot` when released and a "long" press call `factoryReset` on release, but...

To change the speed of a flashing LED as each stage is passed, have your "normal" callback function do something like this:

```cpp
void progress(H4GPIOPin* p){
    H4GM_PIN(Multistage); // Create the correct pointer type in 'pin'
    Serial.print("progress ");Serial.print(pin->stage);Serial.print(" held ");Serial.println(pin->held);
    // case stage = 0 is up to 2 seconds, do nothing 
    switch(pin->stage){
        case 0:
            break;
        case 1:
            h4fc.flashLED(250);
            break;
        case 2:
            h4fc.flashLED(50);
            break;
        default:
            Serial.printf("Something went horribly wrong stage=%d\n",pin->stage);
    }
}
```

For simplicity the code calls another plugin [H4P_FlasherController](h4fc.md) to seamlessly flash the LED and change its rate. The table can have as many entries as you like, the actual call will be:

```cpp
// 15 is mS debounce time
  h4gm.Multistage(USER_BTN,INPUT,ACTIVE_LOW,15,stageMap,progress);

```

### Additional fields for Multistage

`int       stage; // current table entry not exceeding held time`

`uint32_t        held; // Number of microseconds GPIO was held ON`

[Example Code](../examples/GPIO/H4GM_Multistage/H4GM_Multistage.ino)

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)