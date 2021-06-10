![H4P Logo](../assets/WiFiLogo.jpg)

# H4P_RemoteUpdate

## Shortname rupd

Updates the app with a later version from a remote update server. Before using this plugin, users should read both

* [Generic Binaries](generix.md) :heavy_check_mark:
* [Running a remote update server](rusrv.md)

Users may also find this Youtube video helpful, look for the red label "Expert" series [1. Running a remote update server on Nide-RED](https://youtu.be/zNjafa-4QEw)
helpful

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
H4P_RemoteUpdate rupd(...
```

This plugin is a "singleton" - there may be only one single instance of it in the app. 
It may be instantiated as any name the user chooses, prefix all API calls below with than name.

# Dependencies

[H4P_WiFi](h4wifi.md)

# Commands Added

* `h4/rupd/both` // start FSS update, followed by FW update process
* `h4/rupd/fw` // start firmware update process
* `h4/rupd/fs` // start file sytem image update process

# Service Commands

`stop` / `start` have no effect

---

# API

## Constructor

```cpp
// Constructor
//
// url remote server format "http://host:port/path/?a=b..." port, path, query all optional
// e.g. "http://myserver.local/update"
H4P_RemoteUpdate(const string& url="");
//
void both(); // start FSS update, followed by FW update process
void fs(); // start firmware update process
void fw(); // start file sytem image update process

```

Many of the sketches in the [XTRAS folder](../examples/XTRAS) show `H4P_RemoteUpdate` in use.

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/es/esparto)