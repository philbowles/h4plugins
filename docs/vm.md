![H4P Flyer](/assets/DiagLogo.jpg) 

# Verbose Messages

## Shortname="vm"

Replaces internal system IDs with human-readable nmames / messages to assist in learning / debugging
Translates:

* error codes
* log types
* task types
* task names

Merely installing the plugin before any other automatically provides the translation(s) without any further ado.

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
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop
H4P_VerboseMessages vm; // should be created BEFORE any others

This plugin is a "singleton" - there may be only one single instance of it in the app. It may be called whatever you choose: prefix all API calls below with `yourchosenname.`

```

## Dependencies

None, but must be included *before* other plugins
## Commands Added

none

---

## API

```cpp
string  getErrorMessage(uint32_t errorCode); // errorCode returned from invokeCmd
string  getLogType(uint32_t logType); // H4P_LOG_* value
string  getTaskType(uint32_t taskType);
string  getTaskName(uint32_t taskId); // any taskId
```

You would normally use these via the following from [Static and Utility functions](statics.md#translating-cryptic-error-codes--system-ids)

```cpp
std::string h4pgetErrorMessage(uint32_t e); // command errors
std::string h4pgetEventName   (H4P_EVENT_TYPE e);
std::string h4pgetTaskType    (uint32_t e); // H4 Timer Task Type e.g. every, once, nTimes etc
std::string h4pgetTaskName    (uint32_t e); // H4 Timer Task name
```

[Simple example sketch](../examples/BASICS/H4P_VerboseMessages/H4P_VerboseMessages.ino)

[Task names example](../examples/BASICS/H4_TaskNames/H4_TaskNames.ino)

---

(c) 2020 Phil Bowles h4plugins@gmail.com

* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)