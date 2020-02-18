![H4P Flyer](/assets/DiagLogo.jpg) 

# Task Sniffer (short name="snif")

## Adds task activity dump to H4 Universal Scheduler/Timer.

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---

# What does it do?

Displays a message on the creation ("C"), (re)Scheduling ("S") and deletion ("D") of every task in the H4 queue. It is intended as an advanced debugging aid for experts only and should never normally be used, as it causes major loss of performance.

It requires detailed knowledge of H4 "internals" which are beyond the scope of this document.

---

# Usage

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS
H4P_TaskSniffer h4ts;
```

## Dependencies

none, but must be created *after* [**H4P_SerialCmd**](h4sc.md) if using that plugin

## Commands Added

* h4/show/snif
* h4/snif/exclude/< task ID(s) >
* h4/snif/include/< task ID(s) >

A task ID can be one of:

* Single ID e.g. h4/snif/include/13
* A comma separated list of task IDs T1,T2,T3..etc e.g. h4/snif/include/13,14,15
* A range of task IDs T1-T2, e.g. h4/snif/exclude/13-15

## Callbacks

`const char* giveTaskName(uint32_t n) // You get an ID, you return the name of the matching task`

Allows user to name his own tasks so that they are more easily identifiable in the task dump

## Unloadable

NO: but `h4/snif/exclude/0-99` will stop any display. There will still be a performance "hit".

---

# API

```cpp
// 3x Constructors
H4P_TaskSniffer(); // dump ALL tasks
H4P_TaskSniffer(uint32_t i); // dump only task with ID of <i>
H4P_TaskSniffer(initializer_list<uint32_t> i); // dump any task in list {T1,T2,T3.... etc}

// add Task IDs
void include(initializer_list<uint32_t> i); // add {T1,T2,T3... etc} IDs to "hit list"  
void include(uint32_t i); // add single Task ID to "hit list" 
void include(vector<uint32_t> i); // add std::vector of IDs to "hit list"

// parameters are the same for exclude(...) but task ID(s) are removed from "hit list"
void exclude(uint32_t i);
void exclude(initializer_list<uint32_t> i);
void exclude(vector<uint32_t> i);

void show(); // Display current "hit list"

```

[Example Code](../examples/H4P_TaskSniffer/H4P_TaskSniffer.ino)

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
