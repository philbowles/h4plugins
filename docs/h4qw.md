![H4P Flyer](/assets/DiagLogo.jpg) 
# Queue Warn (short name="qwrn")

## Adds low internal queue warning to H4 Universal Scheduler/Timer

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---
# What does it do?

Calls a user-defined function when H4's internal queue drops below a safe level (determined as a percentage of its capacity).

The function is called with a boolean parameter of `true` when the limit is exceeded and `false` when it returns to a safe level.

It is intended purely for expert debugging and is not normally required or used. It could typically be used to light/extinguish a warning LED to show a fault status.

---

# Usage

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS
H4P_QueueWarn h4qw(...
```

## Dependencies

none, but must be created after [**H4P_SerialCmd**](h4sc.md) if using that plugin

## Commands Added

* h4/show/qwrn
* h4/qwrn/pcent/< some sensible percentage>

## Callbacks

```cpp
void onQueueWarning(bool) // called after switch state changes
```

## Unloadable

YES:    No further low-queue warnings will be received.

---

# API

```cpp
//  constructor:
//  f = name of function
//  limit = "safe" percentage
H4P_QueueWarn(function<void(bool)> onQueueWarning,uint32_t _limit=50);

void        show(); // Display current setting (converted to safe numer of tasks)
void        pcent(uint32_t pc); // Set new "safe" percentage
```

[Example code](../examples/H4P_QueueWarn/H4P_QueueWarn.ino)

----
(c) 2020 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Blog](https://8266iot.blogspot.com)
* [Facebook Esparto Support / Discussion](https://www.facebook.com/groups/esparto8266/)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook IOT with ESP8266 (moderator)}](https://www.facebook.com/groups/1591467384241011/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)
