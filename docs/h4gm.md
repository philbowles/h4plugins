![H4P Flyer](/assets/GPIOLogo.jpg) 

# H4P_GPIOManager

## Shortname gpio

Provides easy-to-use "strategies" for many of the common behaviours of buttons, switches and sensors, For example it manages debouncing, decoding of rotary encoders among its 22 different GPIO types. It does all the "heavy lifting" in one line of code and simply calls the user when an interesting event occurs on the GPIO pin.

You need to have read [GPIO Handling](gpio.md) before using this plugin.

---

## Contents

* [Usage](#usage)
* [Dependencies](#dependencies)
* [Commands Added](#commands-added)
* [Events Emitted](#s-emitted)
* [Events Listened for](#s-listened-for)
* [Tasks Run](#tasks-run)
* [Service Commands](#service-commands)
* [API](#api)

---

# Usage

```cpp
H4P_GPIOManager h4gm;
```

This plugin is a "singleton" - there may be only one single instance of it in the app. 
It must be called `h4gm`, prefix all API calls below with `h4gm.`

# Dependencies

N/A
### Commands Added

N/A

*TODO* add commands to e.g. throttle pins at run-time

# Events Listened for

`H4P_EVENT_HEARTBEAT`

# Events Emitted

`H4P_EVENT_BACKOFF`

# Tasks Run

| Symbolic Name | Short Name | Type | Singleton | Purpose |
| :----------   | :--- | :--- | :-------: | :---    |
|H4P_TRID_DBNC|DBNC|once||Debounce timer|
|H4P_TRID_MULT|MULT|once||MultistagePin stage timer|
|H4P_TRID_POLL|POLL|every||PollingPin timer|
|H4P_TRID_RPTP|RPTP|every||RepeatingPin timer|
|H4P_TRID_TRIG|TRIG|once||RetriggeringPin timeout|

# Service Commands

`stop` will immediately end all pin activity

`start` will re-enable pin activity after a stop

## Callback functions

```cpp
void pinChange(H4GPIOPin* ptr);
```

***N.B.*** `H4GM_PIN( pin type )` should be called which takes `ptr` and creates `pin` which is cast to the correct pointer type for the pin type.

See [Accessing the pin object](gpio.md#accessing-the-pin-object-in-code) for more details

---

# API

```cpp
/*
Common parameters:

uint8_t p = GPIO pin number
uint8_t mode = INPUT, INPUT_PULLP etc as in Arduino pinMode
H4GM_SENSE sense = either ACTIVE_HIGH or ACTIVE_LOW
uint32_t dbTimesMS = milliseconds debounce time
pinChange = either a lambda or "normal" function taking one parameter, an H4GPIOPin* This need to be converted to the correct pin type using H4GM_PIN( pin type ) macro before use

Constructor:
*/
H4P_PIOManager();
//
//  General-purpose
//
H4GPIOPin* isManaged(uint8_t p); // returns a pin pointer if this pin has a strategy defined, else nullptr
uint32_t logicalRead(uint8_t p); // returns LOGICAL value of pin
void logicalWrite(uint8_t p,uint8_t l); // set LOGICAL value of pin
void throttle(uint8_t p,uint8_t v); // limit rate of throughput per second
void toggle(uint8_t p); // reverse current LOGICAL state
//
//      Strategies
//
AnalogAveragePin*   AnalogAverage(uint8_t p,uint32_t freq,uint32_t nSamples,H4GM_FN_EVENT pinChange);
nalogThresholdPin*  AnalogThreshold(uint8_t p,uint32_t freq,uint32_t threshold,H4GM_COMPARE compare,H4GM_FN_EVENT pinChange);//
AnalogThresholdPin* AnalogThresholdSource(uint8_t p,uint32_t freq,uint32_t threshold,H4GM_COMPARE compare);//
CircularPin*        Circular(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,uint32_t nStages,H4GM_FN_EVENT pinChange);//
DebouncedPin*       Debounced(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4GM_FN_EVENT pinChange);//
DebouncedPin*       DebouncedSource(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs);//
EncoderPin*         Encoder(uint8_t pA,uint8_t pB,uint8_t mode,H4GM_SENSE sense,H4GM_FN_EVENT);
EncoderPin*         Encoder(uint8_t pA,uint8_t pB,uint8_t mode,H4GM_SENSE sense,int&);
EncoderPin*         EncoderSource(uint8_t pA,uint8_t pB,uint8_t mode,H4GM_SENSE sense);
EncoderAutoPin*     EncoderAuto(uint8_t pA,uint8_t pB,uint8_t mode,H4GM_SENSE sense,int vMin,int vMax,int vSet,uint32_t vIncr,H4GM_FN_EVENT);
EncoderAutoPin*     EncoderAuto(uint8_t pA,uint8_t pB,uint8_t mode,H4GM_SENSE sense,int vMin,int vMax,int vSet,uint32_t vIncr,int&);
FilteredPin*        Filtered(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint8_t filter,H4GM_FN_EVENT pinChange);//
LatchingPin*        Latching(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4GM_FN_EVENT pinChange);//
LatchingPin*        LatchingSource(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs);//
MultistagePin*      Multistage(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4GM_STAGE_MAP stageMap,H4GM_FN_EVENT pinChange);//
OutputPin*          Output(uint8_t p,H4GM_SENSE sense,uint8_t initial,H4GM_FN_EVENT pinChange=nullptr);// FIX ptr type
PolledPin*          Polled(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t frequency,uint32_t isAnalog,H4GM_FN_EVENT pinChange);//
PolledPin*          PolledSource(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t frequency,uint32_t isAnalog);//
RawPin*             Raw(uint8_t p,uint8_t mode,H4GM_SENSE sense,H4GM_FN_EVENT pinChange);//
RawPin*             RawSource(uint8_t p,uint8_t mode,H4GM_SENSE sense);//
RepeatingPin*       Repeating(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,uint32_t frequency,H4GM_FN_EVENT pinChange);//
RetriggeringPin*    Retriggering(uint8_t _p, uint8_t _mode,H4GM_SENSE sense,uint32_t timeout, H4GM_FN_EVENT _pinChange);
RetriggeringPin*    RetriggeringSource(uint8_t _p, uint8_t _mode,H4GM_SENSE sense,uint32_t timeout);
SequencedPin*       Sequenced(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4GM_FN_EVENT pinChange); //
TimedPin*           Timed(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4GM_FN_EVENT pinChange); //
```

`EncoderAuto` has additional specific API:

```cpp
//
// Assuming we have EncoderAutoPin* eap=EncoderAuto(....
//
eap->setValue(uint32_t v); // manually set value: must be min < v < max
eap->setMin() // set position to minimum value: "fully anticlockwise"
eap->setMax() // set position to maximum value: "fully clockwise"
eap->setPercent(uint32_t pc) // manually set value to % of max - min
eap->setCenter() // manully set the value to the mid-point (max - min / 2)
```

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)