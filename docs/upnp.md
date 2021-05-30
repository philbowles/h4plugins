![H4P Flyer](/assets/WiFiLogo.jpg) 

# H4P_UPNPServer

## Shortname upnp

The primary purpose of H4P_UPNPServer is to provide Amazon Alexa voice control and pass the voice commands to either a [H4P_BinarySwitch](things.md) or a [H4P_BinaryThing](things.md). A beneficial side-effect is that the device also appears in the Windows10 "Network Explorer" which provides a quick route to the built-in webserver.

![upnp](../assets/upnp2.jpg)

---

## Contents

* [Usage](#usage)
* [Dependencies](#dependencies)
* [Commands Added](#commands-added)
* [Service Commands](#service-commands)
* [API](#api)

---

# Usage

This plugin is a "singleton" - there may be only one single instance of it in the app. 
It may be instantiated as any name the user chooses, prefix all API calls below with than name.

```cpp
H4P_UPNPServer alexa(...
```

# Dependencies

[H4P_WiFi](h4wifi.md)

and *either*

[H4P_BinarySwitch](things.md)

or

[H4P_BinaryThing](things.md)

# Commands Added

* `name/N` payload N is the new "Friendly name by whcih Alexa knows the device
  
# Service Commands

`stop` / `start` have no effect

## Callback functions

N/A

---

# API

```cpp
/* constructor:
name = Alexa / UPNP "Friendly Name"
*/
H4P_UPNPServer(const string& name="");
//
void friendlyName(const string& name); // change Friendly name
```

Many of the sketches in the [XTRAS folder](../examples/XTRAS) show `H4P_UPNPServer` in use.

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)