![H4P Flyer](../assets/GPIOLogo.jpg) 

# H4P_BinarySwitch, H4P_BinaryThing, H4P_ConditionalSwitch, H4P_ConditionalThing

## Provide on/off/toggle/switch/auto commands to a GPIO (...Switch) or functional object (...Thing)

## Shortname "onof"

You need to be familiar with the terminology used by these Plugins, you must read [Things and Switches](things.md) first.

---

## Contents
* [Usage](#usage)
* [Dependencies](#dependencies)
* [Commands Added](#commands-added)
* [Service Commands](#service-commands)
* [API](#api)
* [Relevant Youtube Videos](#relevant-youtube-videos)

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

Only *one* of the above may exist in the app. The result of using more than one is undefined / unsupported

---

# Dependencies / Interactions

* [H4P_PinMachine](h4pm.md) - must be declared before the ...Switch / ...Thing
    
* [H4P_Wifi](h4wifi.md) - adds "Big red switch" and autoOff input field. If The `Conditional...` variants are used it also add a boolean LED to the webUI to denote the current condition.

* [H4P_AsyncMQTT](h4mqtt.md) - publishes `state` on change

---

# Commands Added

* `h4/auto/T` (automatic off time T milliseconds)
* `h4/off`
* `h4/on`
* `h4/switch/N` (payload N = 1 or 0)
* `h4/toggle` (invert current state)
* `h4/state` (report state) [ if LOG_MESSAGES is defined in [config_plugins.h](../src/config_plugins.h) ]

---
# Service Commands

`stop` / `start` have no effect

---

# API

## Constructors

```cpp
/*
 Common parameters:

H4BS_FN_SWITCH f; // "Thing" function => void myThing(bool onoroff);
H4_FN_CPRED predicate // function whose return value defines whether the device should accept / reject commands. bool predicateFunction()
bool initial; // initial internal state ON or OFF
uint32_t auto; // auto-off timer (milliseconds) 0 = never
*/
H4P_BinaryThing(H4P_FN_VB thingFunction,bool initial=OFF,uint32_t timer=0);
H4P_ConditionalThing(H4P_FN_VB thingFunction,H4_FN_CPRED predicate,bool initial=OFF,uint32_t timer=0): 
/*
 parameters for switches
  uint8_t pin; // GPIO pin number
  H4GM_SENSE sense; // whether this GPIO is ON when ACTIVE_HIGH or ACTIVE_LOW
  color = the color of the condition bool LED in the webUI

enum H4P_UILED_COLOR {
    H4P_UILED_RED,
    H4P_UILED_ORANGE,
    H4P_UILED_YELLOW,
    H4P_UILED_GREEN,
    H4P_UILED_BLUE,
    H4P_UILED_BI       // ON=GREEN, OFF=RED
};
*/
H4P_BinarySwitch(uint8_t pin,H4PM_SENSE sense,uint8_t color=H4P_UILED_BI,uint32_t initial=OFF,uint32_t timer=0);
H4P_ConditionalSwitch(H4_FN_CPRED predicate,uint8_t pin,H4PM_SENSE sense,uint8_t color=H4P_UILED_BI,uint32_t initial=OFF,uint32_t timer=0):
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

(Only when `Conditional...` variant used)

```cpp
void syncCondition();
```

Synchronise webUI view of "Condition" with its true state.

This should be called whenever the value returned by the predicate function changes. Only the user can know what criteria constitute the condition, and thus when its state on the webUI needs to be updated. It is essential therefore, that the user calls this function at the relevant time in the life of the `Conditional...` plugin to keep the webUI "in sync".

---

# Example Sketches

### Things

* [BinaryThing](../examples/05_THINGS_SWITCHES/H4P_BinaryThing/H4P_BinaryThing.ino)
* [BinaryThing with Auto-Off](../examples/05_THINGS_SWITCHES/H4P_BinaryThingAutoOff/H4P_BinaryThingAutoOff.ino)
* [ConditionalThing](../examples/05_THINGS_SWITCHES/H4P_ConditionalThing/H4P_ConditionalThing.ino)
* [Advanced: LED Traffic Signal Sequencer](../examples/05_THINGS_SWITCHES/H4P_TrafficLights/H4P_TrafficLights.ino)

### Switches

* [BinarySwitch](../examples/05_THINGS_SWITCHES/H4P_BinarySwitch/H4P_BinarySwitch.ino)
* [BinarySwitch with Multifunction button](../examples/05_THINGS_SWITCHES/H4P_BinarySwitchMfnb/H4P_BinarySwitchMfnb.ino)
* [BinarySwitch with MQTT](../examples/05_THINGS_SWITCHES/H4P_BinarySwitchMQTT/H4P_BinarySwitchMQTT.ino)
* [ConditionalSwitch](../examples/05_THINGS_SWITCHES/H4P_ConditionalSwitch/H4P_ConditionalSwitch.ino)
* [Advanced: Real-world ConditionalSwitch (Light-dependent Alarm)](../examples/XTRAS/H4P_Generic_SQUAWK/H4P_Generic_SQUAWK.ino)

---

# Relevant Youtube Videos

 * [11: Internet of Things ain't what they used to be](https://youtu.be/fLUKDAboAU8)
 * [12: Internet of Things can only get better](https://www.youtube.com/watch?v=sy7xHuKghrM)

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/es/esparto)