![H4P Flyer](/assets/DiagLogo.jpg) 
# H4P_HeapWarn

## Shortname hwrn

Calls a user-defined function when H4P's free heap drops below a safe level (determined as a percentage of its starting value).

The function is called with a boolean parameter of `true` when the limit is exceeded and `false` when it returns to a safe level.

It is intended purely for debugging and is not normally required. It could typically be used to light/extinguish a warning LED to show a fault status.

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
H4P_HeapWarn h4hw(...
```

This plugin is a "singleton" - there may be only one single instance of it in the app. 
It may be instantiated as any name the user chooses, prefix all API calls below with `myChosenName.`

# Dependencies

N/A

### Commands Added

* `h4/hwrn/pcent/N` // payload N = some sensible percentage

# Events Listened for

N/A

# Events Emitted

N/A

# Tasks Run

| Symbolic Name | Short Name | Type | Singleton | Purpose |
| :----------   | :--- | :--- | :-------: | :---    |
|N/A| | |||

# Service Commands

`stop` has no effect

`start` has no effect

# Callback functions

```cpp
void onHeapWarning(bool) // called after switch state changes
```

---

# API

```cpp
//  constructor:
//  onHeapWarning callback function
//  limit = "safe" percentage
H4P_HeapWarn(function<void(bool)> onHeapWarning,uint32_t _limit=50);

void        pcent(uint32_t pc); // Set new "safe" percentage
```

[Example code](../examples/DIAGNOSTICS/H4P_HeapWarn/H4P_HeapWarn.ino)

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)