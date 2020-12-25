![H4P Flyer](/assets/DiagLogo.jpg) 

# Command Errors (short name="cerr")

# Adds error-code to meaningful message translation to SerialCmd plugin for H4 Universal Scheduler/Timer.

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---

# What does it do?

Translates numeric codes into meaningful messages for:

* error codes
* log types
* task types
* task names

For example:

```cpp
H4_INT_MAP  cmdErrors={
    {H4_CMD_OK,"OK"},
    {H4_CMD_UNKNOWN,"Unknown cmd"},
    {H4_CMD_TOO_FEW_PARAMS,"Too few parameters"},
    {H4_CMD_TOO_MANY_PARAMS,"Too many parameters"},
    {H4_CMD_NOT_NUMERIC,"Numeric value expected"},
    {H4_CMD_OUT_OF_BOUNDS,"Value out of range"},
    {H4_CMD_NAME_UNKNOWN,"Name not known"},
    {H4_CMD_PAYLOAD_FORMAT,"Incorrect Payload Format"}
};
```

Merely installing the plugin before any other automatically provides the translation(s) without any further ado.

---

# Usage

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop
H4P_CmdErrors h4ce; // must be created BEFORE any others


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

[Example Code](../examples/BASICS/H4P_CmdErrors/H4P_CmdErrors.ino)

---

(c) 2020 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Blog](https://8266iot.blogspot.com)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook IOT with ESP8266 (moderator)}](https://www.facebook.com/groups/1591467384241011/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)
