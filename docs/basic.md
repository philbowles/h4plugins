![NODE PINK](../assets/nodepink.jpg)
![H4P Flyer](../assets/GPIOLogo.jpg)

# GPIO Handling and "NODE-PINK": basic flows

The simplest NODE-PINK flows and GPIO strategies - read [GPIO Handling and "NODE-PINK Introduction"](nodepink.md) before continuing

---

# Contents

* [Basic predefined flows](#basic-predefined-flows) 
  * [Common Parameters](#common-parameters)
* [h4pRaw](#h4pRaw) 
* [h4pFiltered](#h4pFiltered)
* [h4pDebounced](#h4pDebounced)
* [h4pTactless](#h4pTactless)
* [h4pCounting](#h4pCounting)
* [h4pCircular](#h4pCircular)
* [h4pLatching](#h4pLatching)
* [h4pTimed](#h4pTimed)
* [h4pRepeating](#h4pRepeating)
* [h4pPolled](#h4pPolled)
* [h4pRetriggering](#h4pRetriggering)
* [h4pMultistage](#h4pMultistage)
* [h4pMultifunctionButton](#h4pmultifunctionbutton)

# Essential further reading:

* [GPIO/NODE-PINK: a logical approach](logphys.md)
* [GPIO/NODE-PINK: rotary encoder flows](encoders.md)
* :building_construction: [GPIO/NODE-PINK: analog flows](analog.md)
* :building_construction: [GPIO/NODE-PINK: advanced techniques](nodepinkadv.md)

---

# Basic Predefined Flows

These are listed in order of increasing pipeline complexity, as many of the more advanced GPIO functions "build on" earlier ones. For example, many GPIO functions need to start with a clean, debounced input, so their pipeline will always start the same as a simple `h4pDebounced` and then add extra nodes to further modify the behaviour.

Thus the order is a *logical* one, so that as you step through it you will find it much easier to understand the subsequent flows

## Common Parameters

To save time and space, parameters that are common to many flows are mentioned here once only.

* `uint8_t` p // The GPIO pin number
* `uint8_t` m // The mode as per Arduino's `pinMode`: either `INPUT` or `INPUT_PULLUP`
* `H4PM_SENSE` s // Whether `ACTIVE_HIGH` or `ACTIVE_LOW` 

For a discussion of active low vs high and the difference between logical and physical pin values, read [Logical vs Physical GPIO](logphys.md). In brief: `ACTIVE_HIGH` inputs are "ON" / `true` when physically reading Vcc 3.3v or "digital 1". `ACTIVE_LOW` inputs are "ON" / `true` when physically reading GND 0v or "digital 0". Be aware that many boards have builtin-leds and/or buttons that are `ACTIVE_LOW`

* `uint32_t` dbTime // the number of milliseconds to debounce the input

---

# h4pRaw

More "an absence of flow", it has a single node, `npPUBLISHVALUE` giving the effect that every single transition sends a `H4PE_GPIO` event to the user's code, i.e. the signal is "raw" and unprocessed.

Pipeline: `npPUBLISHVALUE`

```cpp
h4pRaw(uint8_t p,uint8_t m,H4PM_SENSE s);
```

[Example Sketch](../examples/01_GPIO_PIN_MACHINE/PM_01_Raw/PM_01_Raw.ino)

---

# h4pFiltered

Is essentially an `h4pRaw` with only one type of transition being passed through. `uint8_t filter` is either `HIGH` or `LOW` and dictates which type of transitions are passed: the others are blocked. Specifying `HIGH` for example will produce a (probably) "bouncy" stream of 1's.

Pipeline:  `npFilter{filter}`->`npPUBLISHVALUE`

```cpp
h4pFiltered(uint8_t p,uint8_t m,H4PM_SENSE s,uint8_t filter);
```

[Example Sketch](../examples/01_GPIO_PIN_MACHINE/PM_02_Filtered/PM_02_Filtered.ino)

---

# h4pDebounced

Debounces the input and sends event 1x per "clean" transition, given `dbTime` of debounce time.

Pipeline: `npSmooth{dbTime}`->`npPUBLISHVALUE`

```cpp
h4pDebounced(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t dbms);
```

[Example Sketch](../examples/01_GPIO_PIN_MACHINE/PM_03_Debounced/PM_03_Debounced.ino)

---

# h4pTactless

Converts a typical bouncy "tact" button into a clean, debounced "single-shot" button that "fires" exactly once on the "up-stroke". Useful for real-world events that just need to be fired: a simple "stab" of the button makes *something* happen: your code defines what that is.

The single upstroke `H4PE_EVENT` `msg` will be logical `OFF`, This will always be - by defintion - the opposite of its declared `sense`.

Pipeline: `npSmooth{dbTime}`->`npFilterINACTIVE`->`npPUBLISHVALUE` (`npFilterINACTIVE` is exactly equivalent to `npFilter{ !sense }`)

```cpp
h4pTactless(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t dbTime);
```

[Example Sketch](../examples/01_GPIO_PIN_MACHINE/PM_04_Tactless/PM_04_Tactless.ino)

---

# h4pCounting

Emits `H4PE_EVENT` with an ever-increasing value in `msg`, i.e. it counts how many clean, debounced transition *pairs* or short "stabs" (as in `h4pTactless`) have occurred.

This may seem initially to be of little practical use, but it serves as the basis of several more obviously useful types when the count is acted upon: A "latching" switch (one that stays in the same state till you poke it again) is simply a `h4pCounting` that counts from 0 to 1 then starts again.

Similary, imagine you need to "cycle" through a list of options on a graphcial menu - a quite common user input pattern for many devices is to repeatedly press/release an up/down "arrow" button...which is simply a `h4pCounting` that counts from 1 to < however many menu options you have > then starts again.

Pipeline: `npSmooth{dbTime}`->`npFilterINACTIVE`->`npPUBLISHSIGE`

 (sigE [ short for "sigma Events"] is an internal value held by each pin of the all-time total count of events sent. *This* is the value emitted in the `H4PE_EVENT` rather than the more usual instantaneous actual physical/logical value 0 or 1, since that has no use / meaning in this context)

```cpp
h4pCounting(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t dbTime);
```

[Example Sketch](../examples/01_GPIO_PIN_MACHINE/PM_05_Counting/PM_05_Counting.ino)

---

# h4pCircular

This is one of those GPIOs mentioned in `h4pCounting` (above) that count from 1 to `uint32_t` N and then repeats. It Emits `H4PE_EVENT` with an ever-increasing value in `msg`, i.e. it counts how many clean, debounced transition *pairs* or short "stabs" (as in `h4pTactless`) have occurred, until N is reached, whereupon it returns to 1.

Pipeline: `npSmooth{dbTime}`->`npFilterINACTIVE`->`npPUBLISHCYCLE{N}`

 ("cycle" is internal value *logically* equivalent to sigE % N: it's the current step from 1 to N, hence *this* is the value emitted in the `H4PE_EVENT` rather than the more usual instantaneous actual physical/logical value 0 or 1, since that has no use / meaning in this context)

 If N == 3, subsequent `msg` values will be 1,2,3,1,2,3,1,2... etc

```cpp
h4pCircular(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t dbTime,uint32_t N);
```

[Example Sketch](../examples/01_GPIO_PIN_MACHINE/PM_06_Circular/PM_06_Circular.ino)

---

# h4pLatching

This is one of those GPIOs mentioned in `h4pCounting` (above) that count from 0 to 1. It Emits `H4PE_EVENT` with ON after the first clean, debounced transition *pair* or short "stab" (as in `h4pTactless`) and remains in that state until a subsquent "stab" sends an OFF.

It therefore "flip-flops" between 1 and 0 on every subsequent press/release and effectively converts a cheap "tact" button into a "latching" or "toggling" switch.

Pipeline: `npSmooth{dbTime}`->`npFilterINACTIVE`->`npFLIPFLOP`->`npPUBLISHVALUE`

 (`npFLIPFLOP` is *logically* equivalent to sigE % 2, or `npPUBLISHCYCLE{2}` but more efficient than either)

```cpp
h4pLatching(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t dbTime);
```

[Example Sketch](../examples/01_GPIO_PIN_MACHINE/PM_07_Latching/PM_07_Latching.ino)

---

# h4pTimed

Emits `H4PE_EVENT` with a `msg` value of the number of milliseconds the button was held down for.

Pipeline: `npSmooth{dbTime}`->`npPUBLISHDELTA`

 (`delta` is an internal value measuring the time difference between two subsequent entries to the node)

```cpp
h4pTimed(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t dbTime);
```

[Example Sketch](../examples/01_GPIO_PIN_MACHINE/PM_08_Timed/PM_08_Timed.ino)

---
# h4pRepeating

This is similar to `h4pTimed` but in addition to emitting `H4PE_EVENT` with a `msg` value of the number of milliseconds once it is released, it also emits an event every (`uint32_t f`) milliseconds.

Imagine that `f` == 1000, i.e. the event will repeat every second while the button is held down and also that the user holds the button down for 3.142 seconds. The events sent will be as follows:

|T(ms)|`msg` value*|
| ---: | ---: |
|1000|1000|
|2000|2000|
|3000|3000|
|3142|3142|

*The values are almost certain *not* to be exact multiples of `f` due to "jitter" within the timing code, In reality it will be more like:

|T(ms)|`msg` value|
| ---: | ---: |
|999|999|
|2001|2002|
|3003|3003|
|3142|3143|

This raises two points:
1. Your code must cope with non-exact multiples of `f`
2. If your code is senstive to the odd millisecond here or there, maybe H4Plugins wasn't the best choice.

Finally, if the button is held for less than `f` then `h4pRepeating` behaves exactly as `h4pTimed`, e.g.:

|T(ms)|`msg` value|
| ---: | ---: |
|786|787|

Pipeline: `npSmooth{dbTime}`->`npDELTAREPEAT{f}`->`npPUBLISHVALUE`

```cpp
h4pRepeating(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t dbTime,uint32_t f);
```

[Example Sketch](../examples/01_GPIO_PIN_MACHINE/PM_09_Repeating/PM_09_Repeating.ino)

---

# h4pPolled

`h4pPolled` emits an event every (`uint32_t f`) milliseconds, *provided that the current value is different from the previously sent value*.

In practical terms, this is ideal in apps where there is a slowly changing value, e.g. daylight to darkness, especially if the value tends to "flutter" around the transition point.

In such cases it is recommended that `f` is large, even of the order of numbers of minutes.

Pipeline: `npPassTimer`->`npVALUEDIFF`->`npPUBLISHVALUE`

 (`npPassTimer` ignores any physical GPIO changes and passes through only synthetic timer-generated events)

```cpp
 h4pPolled(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t f);
```

[Example Sketch](../examples/01_GPIO_PIN_MACHINE/PM_10_Polled/PM_10_Polled.ino)

---

# h4pRetriggering

`h4pRetriggering` emits an event when the GPIO first changes (the `ON` "triggering" event), and then an `OFF` event once (`uint32_t timeout`) milliseconds have elapsed - *unless another triggering event occurs before the timeout*. In that case, the internal clock is restarted. We say the device gets triggered, then may be *re*-triggered an infinite number of times before the `timeout` finally expires, causing the `OFF` event to be sent.

In practical terms, this is emulates the behaviour of a PIR sensor. If connecting to such a devices, make sure you choose a `timeout` value that is greater than any timer in the physical device, whether that is hard-wired or user-selectable. We recommend that if user-selectable it is set to the lowest possible value and that `h4pRetriggering`'s `timeout` value is used as the controlling value.

Pipeline: `npTRIGGER{timeout}`->`npPUBLISHVALUE`

```cpp
h4pRetriggering(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t timeout);
```

[Example Sketch](../examples/01_GPIO_PIN_MACHINE/PM_11_Retriggering/PM_11_Retriggering.ino)

---

# h4pMultistage

It is far easier to understand the `h4pMultistage` once you understand the behaviour of the `h4pRepeating` GPIO, since the two are very similar in basic function. The difference with `h4pMultistage`  is that you provide it with a table of "threshold" times (known as a "stage map"), e.g.

Your code is then given a "running commentary" as the button is held down. As each threshold is passed, rather than just reporting the hold time, as `h4pRepeating` does, `h4pMultistage` tells you which threshold has just been passed.

When the button is finally released,  rather than just reporting the total hold time, as `h4pRepeating` does, `h4pMultistage` tells your code the index of the longest threshold that was passed.

Imagine a commercial device with a single button that has to be "paired" with a controller on first install. Usually the user needs to hold down the button for longer than a few seconds, at which point an LED will light or change color etc. He then releases the button to put the device into pairing mode.

The `h4pMultistage` provides that exact functionality, allowing as many stages ( or different actions per different hold times) as required.

Now also imagine you want to define

* A "short" press which is a hold time of anything up to 2 seconds
* A "medium" press of anything between 2 and 5 seconds
* A "long" pres which is anything over 5 seconds

```cpp
H4PM_STAGE_MAP breaks{2000,5000};
```

You have effectively defined 3 "stages" (short/medium/long) separated by threshold values of 2000 and 5000 seconds. We will call those stages 0,1 and 2. Now imagine the user holds down the button for just over 6 seconds:

The events emitted would be as follows:

|Hold Time (ms)|Meaning|`msg` value|
| ---: | ---: | ---: |
|2000|Stage 1 entered|-1|
|5000|Stage 2 entered|-2|
|6142|Release after stage 2(long press)|2|

When held for just over 3 seconds:

|Hold Time (ms)|Meaning|`msg` value|
| ---: | ---: | ---: |
|2000|Stage 1 entered|-1|
|3142|Release after stage 1(medium press)|1|

Finally, when held for 1.5 seconds:

|Hold Time (ms)|Meaning|`msg` value|
| ---: | ---: | ---: |
|1500|Release in stage 0(short press)|0|

Summarising, you get an increasingly negative index each time a new (longer!) stage is entered - this allows you to change LED color or speed up its flashing etc.

You then get a single positive index according to the stage the GPIO was in when the button was released.

Pipeline:  `npSmooth{dbTime}`->`npDELTAREPEAT{f}`->`npSTAGEMANAGER{sm}`

```cpp
h4pMultistage(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t dbTime,H4PM_STAGE_MAP stageMap);
```

[Example Sketch](../examples/01_GPIO_PIN_MACHINE/PM_12_Multistage/PM_12_Multistage.ino)

---

# h4pMultifunctionButton

You need to understand the `h4pMultistage` first, since this is simply a 3-stage version with thresholds at 2000 and 5000 milliseconds with predefined actions as follows:

* Release in Stage 0: perform default on/off action by toggling Global Variable `state`
* Release in Stage 1: Reboot MCU
* Release in Stage 2: Factory Reset MCU
* Hold time enters Stage 1: medium-paced flashing of builtin LED
* Hold time enters Stage 2: fast-paced flashing of builtin LED

This forms the primary input / control mechanism for the majority of H4Plugins WiFi / MQTT examples and demo Apps. As such it is featured in many example sketches 
Pipeline: see `h4pMultistage`

```cpp
h4pMultifunctionButton(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t dbTime);
```

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)