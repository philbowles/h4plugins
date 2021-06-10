![H4P Flyer](../assets/WiFiLogo.jpg) 

# H4P_GateKeeper

## Shortname gate

Manages the detection of network presence events which occur when nominated devices join/leave the network

For more information on how to use this plugin, see [Network Presence Detection](presence.md) 

---

## Contents

* [Usage](#usage)
* [Dependencies](#dependencies)
* [Commands Added](#commands-added)
* [Service Commands](#service-commands)
* [API (for "Roamers")](#api-for-roamers)

---

# Usage

This plugin is a "singleton" - there may be only one single instance of it in the app. 
It may be instantiated as any name the user chooses.

```cpp
H4P_GateKeeper youcantcomeinwithoutabelton();
```

# Dependencies

[H4P_WiFi](h4wifi.md)

# Commands Added

N/A

# Service Commands

`stop` / `start` have no effect

## Callback functions

N/A

---

# API (for "Roamers")

`H4P_GateKeeper` itself has no API, it merely manages and co-ordinates objects know as "Roamers" which represent various different device types whose joining / leaving can be detected.

Individual Roamers have different conctructors, but they all have a common API function:

```cpp
string getIP(); // returns empty string if not in network
```

## Roamers


|Type of device|Identifying features|Example|H4Plugins "Roamer" type|
| :--- | :--- | :--- | :--- |
|MDNS (.local) device| protocol / service attributes|tcp,http|`h4pRoamingDotLocal`|
|H4 device|device name|bedroom|`h4pRoamingH4`|
|any|IP address|192.168.1.42|`h4pRoamingIP`|
|UPNP device|UPNP Tag, value|USN,uuid:Socket-1_0-upnpF9198B|`h4pRoamingUPNP`|

### h4pRoamingDotLocal

```cpp
/* Constructor
name = unique name to identify device to user
service = MDNS service type e.g. tcp or udp etc
protocol = MDNS protocol e.g. hrttp
*/
h4pRoamingDotLocal(const string& name,const string& service,const string& protocol);
```
[Example Sketch 1](../examples/PRESENCE/H4P_PresenceDotLocal/H4P_PresenceDotLocal.ino)

[Example Sketch 2](../examples/PRESENCE/H4P_CrowdedHouse/H4P_CrowdedHouse.ino)

### h4pRoamingH4

```cpp
/* Constructors
name = unique name to identify device to user
*/
h4pRoamingH4(const string& name);
```

[Example Sketch 1](../examples/PRESENCE/H4P_PresenceDetectH4/H4P_PresenceDetectH4.ino)

[Example Sketch 2](../examples/PRESENCE/H4P_CrowdedHouse/H4P_CrowdedHouse.ino)


### h4pRoamingIP

```cpp
/* Constructors
name = unique name to identify device to user
ip = the IP address that device will be recongised by
*/
h4pRoamingIP(const string& name,const string& ip);
h4pRoamingIP(const string& name,const IPAddress& ip);
```

[Example Sketch 1](../examples/PRESENCE/H4P_PresenceDetectIP/H4P_PresenceDetectIP.ino)

[Example Sketch 2](../examples/PRESENCE/H4P_CrowdedHouse/H4P_CrowdedHouse.ino)

Two additional examples show how to link joining / leaving automatically to a default Switch / Thing object (see ["Things" and "Switches"](things.md)) 

While the [joining example](../examples/PRESENCE/H4P_PresenceDetectIP_ONOFF/H4P_PresenceDetectIP_ONOFF.ino)  and the [leaving example](../examples/PRESENCE/H4P_PresenceDetectIP_ONOFF_Inverted/H4P_PresenceDetectIP_ONOFF_Inverted.ino) are coded for `h4pRoamingIP`, the code applies equally to *any* of the Roamers.

### h4pRoamingUPNP

```cpp
/* Constructor
name = unique name to identify device to user
tag = UPNP/SSDP message tag, e.g. USN, MX, LOCATION etc
id = value of the tag considered to be a "match" NB take care, many duplicates my occur with poor choices of tag/id!
*/
h4pRoamingUPNP(const string& name,const string& tag,const string& id);
```

[Example Sketch 1](../examples/PRESENCE/H4P_PresenceDetectUPNP/H4P_PresenceDetectUPNP.ino)

[Example Sketch 2](../examples/PRESENCE/H4P_CrowdedHouse/H4P_CrowdedHouse.ino)

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/es/esparto)