![H4P Flyer](/assets/GPIOLogo.jpg) 

# BinarySwitch, BinaryThing, ConditionalSwitch, ConditionalThing

## Provide on/off/toggle/switch/auto commands to a GPIO (...Switch) or functional object (...Thing)

## Shortname "onof"

You need to be familiar with the terminology used by these Plugins, you must read [Things and Switches](things.md) first.

---

## Contents
* [Usage](#usage)
* [Dependencies](#dependencies)
* [Commands Added](#commands-added)
* [s Emitted](#s-emitted)
* [s Listened for](#s-listened-for)
* [Tasks Run](#tasks-run)
* [Service Commands](#service-commands)
* [API](#api)

---

# Usage

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_BinarySwitch h4onof(...
// OR H4P_BinaryThing h4onof(...
// OR H4P_ConditionalThing h4onof(...
// OR H4P_ConditionalSwitch h4onof(...

```

All of the plugins must exist only as a "singleton" - there must be only one single instance of it in the app. It *must* be called `h4onof`, prefix all API calls below with `h4onof.`

---

# Dependencies

* [H4P_GPIOManager](h4gm.md) 
  
This should be declared before `h4onof`. If this is omitted, it will be dynamically loaded.
  
All plugins add features to [H4P_Wifi](h4wifi.md) (The webUI "Big Red Switch") and [H4P_AsyncMQTT](h4mqtt.md) (auto-publish state) if either of those plugins is used and so  `h4onof` should be placed *after* those.

The `Conditional...` variants also add a read-only boolean to the webUI to denote the current condition.

---

# Commands Added

* `h4/auto/T` (automatic off time T milliseconds)
* `h4/off`
* `h4/on`
* `h4/switch/N` (payload N = 1 or 0)
* `h4/toggle` (invert current state)
* `h4/state` (report state)
* `h4/slave/D,N` (payload D = name of other H4 device on the network, N=1 => add slave; 0 => remove slave). Added only if [H4P_AsyncMQTT](h4mqtt.md) loaded

---

# Events Emitted

* H4P_EVENT_OFF
* H4P_EVENT_ON
  
More information: [Emitters and Listeners](events.md)

---

# Events Listened for

N/A

More information: [Emitters and Listeners](events.md)

---

# Tasks Run

| Symbolic Name | Short Name | Type | Singleton | Purpose |
| :----------   | :---- | :--- | :-------: | :---    |
|H4P_TRID_BTTO|BTTO|Once|:tick:|Auto-off Timer (only if auto > 0)|

---

# Service Commands

`stop` / `start` have no effect

---

# Topics automatically published

If [H4P_AsyncMQTT](h4mqtt.md) is also used, this plugin publishes 

`h4/< your device name >/state` 

with a payload set to the current state whenever the state changes

---

# API

## Constructors

```cpp
/*
 Common parameters:
*/
H4BS_FN_SWITCH f; // "Thing" function => void myThing(bool onoroff);
H4_FN_CPRED predicate // function whose return value defines whether the device should accept / reject commands. It is passed in the current state which it may need to form irs decision => uint32_t predicateFunction(bool currentState)
bool initial; // initial internal state
uint32_t auto; // auto-off timer (milliseconds) 0 = never

H4P_BinaryThing(H4BS_FN_SWITCH f=nullptr,bool initial=OFF,uint32_t auto=0);
H4P_ConditionalThing(H4_FN_CPRED predicate,H4BS_FN_SWITCH f=nullptr,bool initial=OFF,int32_t timer=0);
//
uint8_t pin; // GPIO pin number
H4GM_SENSE sense; // whether this GPIO is ON when ACTIVE_HIGH or ACTIVE_LOW
//
// The optional switch function f runs AFTER the GPIO has been set
H4P_BinarySwitch(uint8_t pin,H4GM_SENSE sense, uint32_t initial,H4BS_FN_SWITCH f=nullptr,uint32_t auto=0):
H4P_ConditionalSwitch(uint8_t pin,H4GM_SENSE sense, uint32_t initial,H4_FN_CPRED predicate,H4BS_FN_SWITCH f=nullptr,uint32_t auto=0):
```

---

## autoOff

```cpp
void autoOff(uint32_t T);
```

Sets auto-off timer to T milliseconds. 0 = Never.

---

## getAutoOff

```cpp
 uint32_t getAutoOff();
 ```

Returns current value of  auto-off timer;

---

## state

```cpp
bool state();
```

Return current internal state

---

## turnOff

```cpp
void turnOff();
```

Set internal state to OFF and emit H4P__OFF

---
## turnOn

```cpp
void turnOn();
```

Set internal state to ON and emit H4P__ON

---
## turn

```cpp
void turn(bool onoroff)
```

Set internal state to ON or OFF and emit H4P__ON or H4P__OFF 

---

( `Conditional...` variants only)

## syncCondition

```cpp
void syncCondition();
```

Synchronise webUI view of "Conditon" with its true state.

This should be called whenever the value returned by the predicate function will have changed by the next time it is called. Only the user can know what criteria constitute the condition, and thus when its state on the webUI needs to be updated. It is essential therefore, that the user calls this function at the relevant time in the life of the `Conditional...` plugin to keep the webUI "in sync".

---

# Example Sketches

## Output Examples

### Things

* [BinaryThing](../examples/THINGS/H4P_BinaryThing/H4P_BinaryThing.ino)
* [BinaryThing with Auto-Off](../examples/THINGS/H4P_BinaryThingAutoOff/H4P_BinaryThingAutoOff.ino)
* [ConditionalThing](../examples/THINGS/H4P_ConditionalThing/H4P_ConditionalThing.ino)
* [Advanced Thing (Traffic Light Sequencer)](../examples/THINGS/H4P_TrafficLights/H4P_TrafficLights.ino)

### Switches

* [BinarySwitch](../examples/THINGS/H4P_BinarySwitch/H4P_BinarySwitch.ino)
* [BinarySwitch with Multifunction button](../examples/THINGS/H4P_BinarySwitchMfnb/H4P_BinarySwitchMfnb.ino)
* [BinarySwitch with MQTT](../examples/THINGS/H4P_BinarySwitchMQTT/H4P_BinarySwitchMQTT.ino)
* [ConditionalSwitch](../examples/THINGS/H4P_ConditionalSwitch/H4P_ConditionalSwitch.ino)
* [Real-world ConditionalSwitch (Light-dependent Alarm)](../examples/THINGS/H4P_Generic_SQUAWK/H4P_Generic_SQUAWK.ino)


## Input Examples

You need to read the [H4P_GPIOManager](h4gm.md) documentation before using these

* [AnalogThresholdSource](../examples/GPIO/H4GM_DebouncedSource/H4GM_DebouncedSource.ino)
* [DebouncedSource](../examples/GPIO/H4GM_DebouncedSource/H4GM_DebouncedSource.ino)
* [EncoderSource](../examples/GPIO/H4GM_EncoderSource/H4GM_EncoderSource.ino)
* [LatchingSource](../examples/GPIO/H4GM_LatchingSource/H4GM_LatchingSource.ino)
* [PolledSource](../examples/GPIO/H4GM_PolledSource/H4GM_PolledSource.ino)
* [RawSource](../examples/GPIO/H4GM_RawSource/H4GM_RawSource.ino)
* [RetriggeringSource](../examples/GPIO/H4GM_RetriggeringSource/H4GM_RetriggeringSource.ino)

You need to read the [H4P_UPNPServer](upnp.md) documentation before using:

* [UPNPServer](../examples/XTRAS/H4P_SONOFF_Basic/H4P_SONOFF_Basic.ino)

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)