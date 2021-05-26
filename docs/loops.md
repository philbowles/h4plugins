![H4P Logo](../assets/DiagLogo.jpg)

# H4P_EmitLoopCount

## Shortname evtq

Emits `H4PE_LOOPS` once per second. This gives a *very* rough estimate of the number of times per second that H4 is able to schedule its task queue.

In an unloaded ESP8266 clocked at 80MHz with an empty queue, this figure is typically ~75000. This allows a (*very*) rough comparison with how "busy" the current app is, and can be helpful in tracking down bugs, performance bottlenecks etc. Do not treat its absolute value nor meaning as particulalry scientific.

***N.B.*** This may require editing of [H4](https://github.com/philbowles/H4) configuration, because providing this facility is in itself a performance "hit" and is therfore usually "compiled out".

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
H4P_EmitLoopCount lpsCounter();
```

This plugin is a "singleton" - there may be only one single instance of it in the app. 
It may be instantiated as any name the user chooses, prefix all API calls below with than name.

# Dependencies

[H4P_EmitTick](tick.md)

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
H4P_EmitLoopCount();
```

[Example Sketch](../examples/03_DIAGNOSTICS/H4P_LoopCount/H4P_LoopCount.ino)

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/es/esparto)