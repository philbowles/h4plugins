![H4P Flyer](../assets/DiagLogo.jpg)

# H4P_Heartbeat

## Shortname beat

Adds "uptime" to the webUI, showing how loing MCu has been powered up

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
H4P_Heartbeat BuddyHolly;
```

This plugin is a "singleton" - there may be only one single instance of it in the app. It may be instantiated as any name the user chooses.

# Dependencies

[H4P_EmitTick](tick.md) 
[H4P_WiFi](h4wifi.md)]

# Commands Added

N/A

# Service Commands

`stop` has no effect

`start` has no effect

# Callback functions

N/A

---

# API

```cpp
// constructors
H4P_Heartbeat();
H4P_Heartbeat(size_t period); // ms at which LED_BUILTIN will be pulsed to show "online" NB ONLY ON MCU with LED_BUILTIN defined

static string secsToTime(uint32_t secs; // returns human readable D HH:MM:SS of arbitray seconds value
static string upTime(); // returns human readable D HH:MM:SS of time since boot
```

[Example code](../examples/06_WEB_UI/WebUI_InputFields/WebUI_InputFields.ino)

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/es