![H4P Flyer](/assets/GPIOLogo.jpg) 

# H4P_MultiFunctionButton

## Shortname mfnb

Turns a momentary 'tact' button (often found on dev boards as "user button") into a multi-function input device. Users should be familiar with the [MultiStage GPIO handling](gpio.md#multistage) (on which mfnb is based) before using this plugin. The button causes one of three behaviours depending on how long it is held down. "Short" "medium" and long" in the following description are user definable.

* A short press (less than 2 seconds) of the button activates the default on / off functionality of either  a [H4P_BinarySwitch](things.md) or a [H4P_BinaryThing](things.md). In this respect it acts as a "source", see * ["Things", "Switches", "Sources" and "Slaves"](things.md)


* A "medium" press over 2 seconds (but less than 5 seconds) starts the associated LED flashing rapidly and when the button is released, the device will reboot.

* A "long" press - anything over 5 seconds - starts the LED flashing extremely rapidly and when the button is released, the device will clear any stored configuration information and then reboot, a process known as a "factory reset".
  
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

This plugin is a "singleton" - there may be only one single instance of it in the app. 
It may be instantiated as any name the user chooses, prefix all API calls below with `myChosenName.`

```cpp
H4P_MultiFunctionButton mfnb(...
```

# Dependencies

[H4P_FlasherController](h4fc.md)

[H4P_GPIOManager](h4gm.md)
and *either*

[H4P_BinarySwitch](things.md)

or

[H4P_BinaryThing](things.md)

# Commands Added

N/A

# Events Listened for

N/A

# Events Emitted

* `H4P_EVENT_FACTORY`
* `H4P_EVENT_REBOOT`

# Tasks Run

| Symbolic Name | Short Name | Type | Singleton | Purpose |
| :----------   | :--------- | :--- | :-------: | :---    |
|N/A| | | | |

# Service Commands

`stop` / `start` have no effect

## Callback functions

N/A

---

# API

```cpp
/* constructor:
parameters have the same meaning as  the common parameters for all pin strategies in GPIOManager
*/
H4P_MultiFunctionButton(
//  the input button
    uint8_t pin,
    uint8_t mode,
    H4GM_SENSE b_sense,
    uint32_t dbTimeMs, // debounce time
);
```

[Example code](../examples/XTRAS/H4P_SONOFF_Basic/H4P_SONOFF_Basic.ino)

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)