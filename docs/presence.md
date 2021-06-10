![H4P Logo](../assets/WiFiLogo.jpg)

# Network Presence Detection

Reacting to the network arrival and departure of "roaming" devices

---

# What does it do?

There is a plugin called [H4P_GateKeeper](gk.md). Your sketch lists identifying fatures of specifc devices, for example your mobile phone, a family member's laptop etc.

The [H4P_GateKeeper](gk.md) emits an `H4PE_PRESENCE` event whenever one of the named roaming devices enters (msg==1) or leaves (msg=0) the network. 

This can be used for example to implement a self-arming alarm system which will switch on whenever your personal phone leaves the network and switches off when your phone is detected again as you arrive back home. It could turn on the outsiude lights at the same time when it is dark.

See Youtube video: [13: Target in Range!: Network Presence Detection](https://www.youtube.com/watch?v=tLeptP3D5fw)

# What can be detected?

|Type of device|Identifying features|Example|H4Plugins "Roamer" type|
| :--- | :--- | :--- | :--- |
|any|IP address|192.168.1.42|`h4pRoamingIP`|
|UPNP device|UPNP Tag, value|USN,uuid:Socket-1_0-upnpF9198B|`h4pRoamingUPNP`|
|MDNS (.local) device| protocol / service attributes|tcp,http|`h4pRoamingDotLocal`|
|H4 device|device name|bedroom|`h4pRoamingH4`|


---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/es