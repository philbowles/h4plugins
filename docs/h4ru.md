![H4P Flyer](/assets/DiagLogo.jpg) 

# Remote Update (short name="rupd")

## Adds OTA update from remote server to H4 Universal Scheduler/Timer.

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---

# What does it do?

Employs the `ESP8266httpUpdate.h` (or `ESPhttpUpdate.h` for ESP32) library to "pull" either SPIFFS or latest firmware update from remote server.
Setting up the server is beyond the scope of this document but is discussed in [Generic Deployment](XTRAS.md)

Also, see the nodejs/express sever which does this for you, or the [NODE-RED H4 Updater flow](advanced.md) in the advanced documentation.

[NodeJS/Express update server](../examples/LOGGING/H4P_HttpMySQLLogger/mysqlrest/routes/index.js)

---

# Usage

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop
H4P_RemoteUpdate h4ru(...
```

## Dependencies

[**H4P_WiFi**](h4wifi.md) if using that plugin

## Commands Added

`h4/rupd/both` // updates to later spiffs(if any) then fimware(if any)
`h4/rupd/spiffs` // updates later spiffs (if any)
`h4/rupd/update` // updates later firmware (if any)

If any of the above succeeds, i.e. there *is* a later version available which is updated, then the device will reboot

---

# API

```cpp
// Constructor url must contain :port if necessary, e.g. "http://192.168.1.4:1880/update"
H4P_RemoteUpdate(const string& url); // runAtStart checks/updates both on startup


void both(); // updates to later spiffs(if any) then fimware(if any)
void spiffs(); // updates later firmware (if any)
void update(); // updates later firmware (if any)
```

[Example Code](../examples/XTRAS/H4P_RemoteUpdate/H4P_RemoteUpdate.ino)

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
