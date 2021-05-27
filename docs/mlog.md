![H4P Logo](../assets/DiagLogo.jpg)

# H4P_MQTTLogger

## Shortname mlog< N >

Registers a handler function with a selected set of events, which publishes the event message to MQTT using a topic name selected by the user.

For information on how to use this plugin, see [Everything is an event: Listeners, Emitters and Handlers](events.md)

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

H4P_MQTTLogger is a multiple-use plugin: there can be many in one app.
They will appear as <base shortname>1, <base shortname>2. <base shortname>3 etc. 
They may be instantiated as any name.

```cpp
H4P_MQTTLogger myML1(...             // internal ref: mlog1
H4P_MQTTLogger myML2(...             // internal ref: mlog2
H4P_MQTTLogger anyNameYouChoose(...  // internal ref: mlog3
```

# Dependencies

[H4P_AsyncMQTT](h4mqtt)

# Commands Added

N/A

# Service Commands

`stop` / `start` have no effect

# API

## Constructor

```cpp
/*
uint32_t filter; Name of the event (or combination of events) to be fowarded to the event handler function
const string& topic; MQTT topic name the event(s) is published to 
*/
H4P_MQTTLogger(const string& topic,uint32_t filter);
```

[Example Sketch](../examples/02_LOGGING/H4P_MQTTLogger/H4P_MQTTLogger.ino)

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)