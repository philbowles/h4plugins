![H4P Logo](../assets/DiagLogo.jpg)

# H4P_LocalLogger

## Shortname llog

`H4P_LocalLogger` reserves a user-defind amount of FS space to create a log file. It then registers a handler function with a selected set of events, which are saved to file `log.csv`. 

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

When the file size exceeds the limit, `H4P_LocalLogger` will `flush` the log, i.e. it will print the contents to the serial monitor and empty the file.

The user needs to choose the limit carefully: Any file that is > half of the free heap will crash the ESP if the user tries to `h4/dump` it, or allows it to grow past the chosen size and cause a `flush` to the monitor.

The safest way to view the `log.csv` file is to simply browse to it: `http://< device ip address >/log.csv`

For information on how to use this plugin, see [Event Listeners, Event Emitters and logging](events.md)

---
# Usage

```cpp
H4P_LocalLogger myLL(...
```

This plugin is a "singleton" - there may be only one single instance of it in the app. It may be instantiated as any name the user chooses.

# Dependencies

N/a

# Commands Added

* `h4/log/clear`
* `h4/log/flush`

# Service Commands

`stop` / `start` have no effect

## Callback functions

N/A

---

# API

```cpp
/* constructor
uint32_t limit; // maximum amount of FS space to use
uint32_t filter; // Name of the event (or combination of events) to be written to the log
*/
H4P_LocalLogger(uint32_t limit=10000,uint32_t filter=H4P_LOG_ALL);

void clear(); // empties the log
void flush(); // show then clear
void show(); // show first chunk of log on Serial
```

[Example Code](../examples/02_LOGGING/H4P_LocalLogger/H4P_LocalLogger.ino)

---

# The Log File

## Format

millis,type,source,message

## Example
89,SVCUP,h4,vm

108,SVCUP,h4,cmd

126,SVCUP,h4,llog

1147,USER,H4P,PING 0

2243,USER,H4P,PING 1

3146,USER,H4P,669

3298,USER,H4P,PING 2

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https: