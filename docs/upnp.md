![H4P Flyer](/assets/GPIOLogo.jpg) 

# H4P_UPNPServer

## Shortname upnp

The primary purpose of H4P_UPNPServer is to provide Amazon Alexa voice control and pass the voice commands to either a [H4P_BinarySwitch](things.md) or a [H4P_BinaryThing](things.md). In this respect it acts as a "source", see * ["Things", "Switches", "Sources" and "Slaves"](things.md). A side-effect is that the device also appears in the Windows10 "Network Explorer" which provides a quick route to the built-in webserver.

![upnp](../assets/upnp2.jpg)

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

This plugin is a "singleton" - there may be only one single instance of it in the app. 
It may be instantiated as any name the user chooses, prefix all API calls below with `myChosenName.`

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
* `host2/D,N` changes both the device hostname (see wifi/host command) and its friendly name. This is useful when using auto-configuration of generic device code , see [Advanced Topics](advanced.md)
  
# Events Listened for

`H4PE_FACTORY`

# Events Emitted

N/A

# Tasks Run

| Symbolic Name | Short Name | Type | Singleton | Purpose |
| :----------   | :--------- | :--- | :-------: | :---    |
|H4P_TRID_NTFY|UNFY|every|:heavy_check_mark:|SSDP Presence notification|
|H4P_TRID_SOAP|SOAP|QF||Proces Alexa SOAP Request|
|H4P_TRID_UDPS|UDPS|nTimesRandom||Jitter for UDP TX burst|

# Service Commands

`stop` / `start` have no effect

## Callback functions

N/A

---

# API

```cpp
/* constructor:
name = Alexa / UPNP "Friendly Name"
onConnect function called when...you guessed it...
onDisconnect etc
*/
H4P_UPNPServer(const string& name="",H4_FN_VOID onConnect=nullptr,H4_FN_VOID onDisconnect=nullptr);
//
void friendlyName(const string& name); // change Friendly name
void setBothNames(const string& host,const string& friendly); // change device hostname and Friendly Name

```

[Example code](../examples/XTRAS/H4P_SONOFF_Basic/H4P_SONOFF_Basic.ino)

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)