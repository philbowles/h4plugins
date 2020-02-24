![H4P Flyer](/assets/DiagLogo.jpg) 

# Command Errors (short name="cerr")

# Adds error-code to meaningful message translation to SerialCmd plugin for H4 Universal Scheduler/Timer.

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---

# What does it do?

Translates command-handling numeric error codes into meaningful messages:

```cpp
{0,"OK"},
{1,"Unknown cmd"},
{2,"Too few parameters"},
{3,"Too many parameters"},
{4,"Numeric value expected"},
{5,"Value out of range"},
{6,"Name not known"},
{7,"Incorrect Payload Format"},
{8,"Prohibited from here"}
```

Merely installing the plugin before [**H4P_SerialCmd**](h4sc.md) automatically provides the translation without any further ado.

If required, it can be used manually after a direct call ( see  [**H4P_SerialCmd**](h4sc.md) ) by calling the single API function below supplying the error code retuned from `h4sc.invokeCmd(...);`

---

# Usage

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS
H4P_CmdErrors h4ce; // must be created BEFORE H4P_SerialCmd
H4P_SerialCmd h4sc;
```

## Dependencies

None, but must be included *before* [**H4P_SerialCmd**](h4sc.md)

## Commands Added

none

### Unloadable

NO

---

## API

```cpp
string      getErrorMessage(uint32_t errorCode);
string      getLogType(uint32_t errorCode); // trnaslate logging type codes into human-readable form
```

[Example Code](../examples/H4P_CmdErrors/H4P_CmdErrors.ino)

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
