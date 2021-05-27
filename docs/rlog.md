![H4P Logo](../assets/DiagLogo.jpg)

# H4P_RemoteLogger

## Shortname rlog

`H4P_RemoteLogger` registers a handler function with a selected set of events, which are saved to sent to a remote url as `POST` request with `application/x-www-form-urlencoded` data fields. 

For information on how to use this plugin, see [Everything is an event: Listeners, Emitters and Handlers](events.md)

## Contents

* [Introduction](#introduction)
* [Usage](#usage)
* [Dependencies](#dependencies)
* [Commands Added](#commands-added)
* [Service Commands](#service-commands)
* [API](#api)

---

# Introduction

The data fields' names will need to be known by the remote server's POST handler.

* device
* type
* source
* msg

---

# Usage

```cpp
H4P_RemoteLogger myLL(...
```

This plugin is a "singleton" - there may be only one single instance of it in the app. It may be instantiated as any name the user chooses.

# Dependencies

[H4P_AsyncHTTP](h4phttp.md)

# Commands Added

N/A

# Service Commands

`stop` / `start` have no effect

## Callback functions

N/A

---

# API

```cpp
/* constructor
// url remote server format "http://host:port/path/?a=b..." can start https://. port, path, query all optional
// e.g. "http://myserver.local:1883"
filter; // Name of the event (or combination of events) to be written to the log
*/
H4P_RemoteLogger(const string& url,uint32_t filter=H4PE_ALL);
```

[Example Code](../examples/02_LOGGING/H4P_RemoteLogger/H4P_RemoteLogger.ino)

See also [MySQL event logger](https://github.com/philbowles/mysqlrest) github repo for a NodeJS / Express demo webserver which will process the events sent by `H4P_RemoteLogger` and store them in a MySQL database

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)