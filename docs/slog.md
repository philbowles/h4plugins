![H4P Logo](/assets/DiagLogo.jpg)

# H4P_SerialLogger

## Shortname slog< N >

Registers a handler function with a selected set of events, which prints the details of the event on the serial monitor.

For information on how to use this plugin, see [Event Listeners, Event Emitters and logging](events.md)

---

# Usage

H4P_SerialLogger is a multiple-use plugin: there can be many in one app.
They will appear as <base shortname>1, <base shortname>2. <base shortname>3 etc. 
They may be instantiated as any name.

```cpp
H4P_SerialLogger mySL1(...             // internal ref: slog1
H4P_SerialLogger mySL2(...             // internal ref: slog2
H4P_SerialLogger anyNameYouChoose(...  // internal ref: slog3
```

# Dependencies

N/a

# Commands Added

N/A

# Events Listened for

Any, depends on filter parameter: see constructor

# Events Emitted

N/A

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

## Constructor

```cpp
/*
uint32_t filter; Name of the event (or combination of events) to be fowarded to the event handler function
*/
H4P_SerialLogger(uint32_t filter);
```

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)