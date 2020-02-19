![H4P Flyer](/assets/DiagLogo.jpg) 
# Persistent Storage (short name="stor")

## Adds persistent storage across reboots to H4 Universal Scheduler/Timer (ESP8266 / ESP32 only)

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---
# What does it do?

Provides a mechanism for storing named / value pairs which are persisted across reboots (via SPIFFS) and automatically available on next boot.

All saved values will be lost on factory reset.

---

# Usage

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS
H4P_SerialCmd h4sc;
H4P_persistentStorage h4ps(...
```

## Dependencies

H4P_SerialCmd

Board must be compiled with an amount of SPIFSS space

## Commands Added

* h4/show/stor
* h4/stor/clear  (erases all stores values)
* h4/stor/get/x (payload x = name of a stored value)
* h4/stor/set/x,y (payload x = name of a stored value,y=new value)

## Callbacks

```cpp
void onChange(string name,string value) // called after stored item changes
```

## Unloadable

NO:

---

# API

H4P_PersistentStorage overloads the [] operator, so that

* `h4ps["myvalue"]="some value";`
* `string mv=h4ps["myvalue"];`

are both valid.

```cpp
//  constructor:
//  f = (optional) name of callback function when any named item changes value
H4P_PersistentStorage(H4P_FN_PSCHANGE f=nullptr);

void clear();   // erase all stored values
void dec(const string& name); // decrement stored int value: no effect on string value
bool exists(const string& name); // true if name has a stored value
string getstring(const string& name); // get named string value
int  getint(const string& name); // get named int value
void inc(const string& name) // decrement stored int value: no effect on string value
void setstring(const string& name,const string& value); // set name = value
void setint(const string& name,int value); // set name = value for integers
void showStore(){ for(auto const& p:psRam); // displays all name/value pairs
```

[Example code](../examples/H4P_persistentStorage/H4P_persistentStorage.ino)

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
