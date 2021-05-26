![H4P Logo](../assets/DiagLogo.jpg)

# H4P_EmitTick

## Shortname 1sec

Emits `H4PE_HEARTBEAT` event once per second. This is synchronous, called on the main loop and *not* an *exact* asynchronous "hard-realtime" second. It is sent when the number of `millis()` is exactly divisible by 1000. Unless your app loads up the queue with badly-designed long-running tasks, it is reasonably accurate.

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
H4P_EmitTick ticktock();
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
H4P_EmitTick();
```

[Example Sketch](../examples/00_START_HERE/04_YourOwnEventHandling/04_YourOwnEventHandling.ino)

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/es/esparto)