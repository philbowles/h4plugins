![H4P Logo](../assets/DiagLogo.jpg)

# H4P_EmitQ

## Shortname evtq

Emits `H4PE_Q` events at a user-defined frequency. The event message contains the string representation of the size of the H4 task Queue.

For information on how to use this plugin, see :gem:[Everything is an event: Listeners, Emitters and Handlers](events.md)

---

## Contents

* [Usage](#usage)
* [Dependencies](#dependencies)
* [Commands Added](#commands-added)
* [Service Commands](#service-commands)
* [API](#api)

---
# Usage

```cpp
H4P_EmitQ queueEmitter(...
```

This plugin is a "singleton" - there may be only one single instance of it in the app. 
It may be instantiated as any name the user chooses, prefix all API calls below with than name.

# Dependencies

N/A

# Commands Added

N/A

# Service Commands

`stop` / `start` have no effect

## Callback functions

N/A

---

# API

## Constructor

```cpp
/*
uint32_t f; event frequency in milliseconds
uint32_t scale; multiply the actual value by this amount - this makes the graph trace much more visible if graphed at the same time as e.g. heap with is usually 5 digits
*/
H4P_EmitQ(uint32_t f=1000,uint32_t scale=1)
```

[Example Sketch 1](../examples/00_START_HERE/04_YourOwnEventHandling/04_YourOwnEventHandling.ino)

[Example Sketch 2](../examples/02_LOGGING/H4P_MQTTLogger/H4P_MQTTLogger.ino)

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/es/esparto)