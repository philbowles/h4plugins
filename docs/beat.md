![H4P Flyer](/assets/DiagLogo.jpg)

# H4P_Heartbeat

## Shortname beat

Adds "Uptime" to the web UI and provides some simple time display conversion functions

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
H4P_Heartbeat h4hb;
```

This plugin is a "singleton" - there may be only one single instance of it in the app. 
It may be instantiated as any name the user chooses, prefix all API calls below with `myChosenName.`

# Dependencies

[H4P_WiFi](h4wifi.md)]

# Commands Added

N/A

# Events Listened for

`H4PE_HEARTBEAT`

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

N/A

---

# API

```cpp
// constructor
H4P_Heartbeat();

static string secsToTime(uint32_t secs; // returns human readable D HH:MM:SS of arbitray seconds value
static uint32_t upSecs(); // returns raw numberup seconds since boot
static string upTime(); // returns human readable D HH:MM:SS of time since boot
```

[Example code](../examples/WEBUI/WebUI_InputFields/WebUI_InputFields.ino)

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)