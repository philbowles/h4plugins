![H4P Logo](/assets/DiagLogo.jpg)

# H4P_EmitHeap

## Shortname heap

Emits `H4PE_HEAP` events at a user-defined frequency. The event message contains the string representation of the amount of free heap space.

For information on how to use this plugin, see [Event Listeners, Event Emitters and logging](events.md)

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
H4P_EmitHeap heapEmitter(...
```

This plugin is a "singleton" - there may be only one single instance of it in the app. 
It may be instantiated as any name the user chooses, prefix all API calls below with `myChosenName.`

# Dependencies

N/A

# Commands Added

N/A

# Events Listened for

N/A

# Events Emitted

`H4PE_HEAP`

# Tasks Run

| Symbolic Name | Short Name | Type | Singleton | Purpose |
| :----------   | :--- | :--- | :-------: | :---    |
|H4P_TRID_HLOG|HLOG|every|:heavy_check_mark:|Event frequency timer|

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
*/
H4P_EmitHeap(uint32_t f=1000);
```

[Example Sketch](../examples/LOGGING/EmittersListeners/EmittersListeners.ino)

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)