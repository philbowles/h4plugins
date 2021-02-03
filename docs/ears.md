![H4P Logo](/assets/DiagLogo.jpg)

# H4P_EventListener

## Shortname ears< N >

Registers a handler function with a selected set of events. 
For information on how to use this plugin, see [Event Listeners, Event Emitters and logging](events.md)

---

# Usage

H4P_EventListener is a multiple-use plugin: there can be many in one app. 
They will appears as <base shortname>1, <base shortname>2. <base shortname>3 etc. 
They may be instantiated as any name.

```cpp
H4P_EventListener myEL1(...             // internal ref: ears1
H4P_EventListener myEL2(...             // internal ref: ears2
H4P_EventListener anyNameYouChoose(...  // internal ref: ears3
```

# Dependencies

N/a

# Commands Added

N/A

# Events Listened for

Any, depends on filter parameter: see constructor

# Tasks Run

| Symbolic Name | Short Name | Type | Singleton | Purpose |
| :----------   | :--------- | :--- | :-------: | :---    |
|N/A| | | | |

# Service Commands

`stop` / `start` have no effect

## Callback functions

```cpp
/*
`pid` is the plugin ID of the plugin that sent or "emitted" this event
`t` is the type of event.
`msg` is optional information about that event.
*/
void handleEvent(H4PID pid,H4P_EVENT_TYPE t,const string& msg);
```

---

# API

## Constructor
/*
filter is Name of the event (or combination of events) to be fowarded to the event handler function
*/

```cpp
H4P_EventListener(uint32_t filter,H4P_FN_EVENTHANDLER handleEvent);
```

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)