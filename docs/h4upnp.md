![H4P Flyer](/assets/GPIOLogo.jpg) 

# UPNP Switch

## Adds UPNP functionality to H4 Universal Scheduler/Timer, allowing control by e.g. Windows Network Explorer, NODE-RED via UPNP node and voice control by Amazon Alexa. Runs on ESP8266/32 only

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---

# What does it do?

![upnp](/assets/upnp.jpg) 

H4P_UPNPSwitch wraps the simple functionality of [H4P_BasicSwitch](/onof.md) into a UPNP device. It emulates a Belkin Wemo device (with a few bells and whistles added).

This enables it to appear in the Windows Network Explore and be switched on and off from your PC. It also allows voice control by Amazon Alexa

---

# Usage

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS

H4P_WiFi h4wifi(...
H4P_AsyncWebServer h4asws(...
H4P_UPNPSwitch h4upnp;
```

## Dependencies


And:

* [H4P_WiFi](h4wifi.md) Plugin

* [H4P_AsyncWebServer](h4asws.md) Plugin

**N.B.** *H4P_UPNPSwitch represents a default action: it cannot be used simultaneously with any other plugin that also defines a default action, e.g. [H4P_BasicSwitch](h4onof.md).*

## Commands Added

* h4/off
* h4/on
* h4/switch/n (payload = 1 or 0)
* h4/toggle (invert current state)
* h4/upnp/name/x (payload x=newUPNPname: causes a reboot )

## Topics automatically published

If [H4P_MQTT](h4mqtt.md) is also used, this plugin publishes `h4/< your device name >/state` with a payload set to the current state whenever the state changes

## Callbacks

```cpp
onSwitch(bool) // called after switch state changes
```

## Trusted Name

*ONOF*

## Unloadable

No

---

# Installation of Windows components

***N.B.** It seems Windows has a bug(!) in handling Wemo UPNP devices. Until this is fixed or a workaraound is found, the only way to activate your H4 device using UPNP is indirectly via MQTT*

Until such time as an installation script is written (soon, I promise :) ) getting the windows functionality is a bit of an ordeal, I'm afraid.

The first thing you need to do is to locate you Arduino library installation folder for H4Plugins, it will be something like `C:\Users\phil\Documents\Arduino\libraries\H4Plugins`

The files you will need are in the `src` subfolder

1. Install Powershell if you dont already have it and set it up so that it can run code without needing admin rights More information [here](https://superuser.com/questions/106360/how-to-enable-execution-of-powershell-scripts) 

2. Install [m2mqtt](https://github.com/eclipse/paho.mqtt.m2mqtt) You may find it easier to first install [nuget.exe](https://www.nuget.org/downloads) and run `nuget.exe install M2Mqtt -o c:\lib`

3. Edit h4p.reg and change the location in the final to match your username  Then right-click on that file and select "Merge" to add it to the registry

---

# API

```cpp
// Constructor
H4P_UPNPSwitch(string,uint8_t pin,H4GM_SENSE sense, uint8_t initial,H4BS_FN_SWITCH f=[](bool){});
// name is the "friendly name" which appears in Windows Network Explorer
// and is also the name used by Amazon Alexa voce control
// pin is the GPIO pin  which gets "switched" when the state changes
// sense is ACTIVE_HIGH or ACTIVE_LOW of the gpio pin, depending on the hardware
// initial is th intial desired logical state ON or OFF
// f is the name of  a user function that gets called after the state change with b set to the current state
//
void friendlyName(const string& name); // sets UPNP friendly name. Causes a reboot
void turnOff();
void turnOn();
void toggle(); // invert state
void turn(bool b); // b = new state: 1 or 0; true/false; ON/OFF

```

[Example Sketch - SONOFF Basic](../examples/H4P_SONOFF_Basic/H4P_SONOFF_Basic.ino)

---

# Advanced Topics

## Device naming

If no name is given in the constructor, it defaults to "upnp XXXXXX" where XXXXXX is the unique chip ID of the device (usually the last 6 characters of the MAC address).

This is useful to enable a single generic sketch to be uploaded to numerous devices without change. Each device should then be sent a `h4/wifi/name` command to give it a "sensible" name. This can be done by any MQTT client using stored messages (or e.g. NODE-RED), so that each device gets its own new name every time it reboots.

## Precedence

The situation is a little different if the device has a name defined in the constructor. Assume this is "firstname". When given a `h4/upnp/name` command with a payload of "secondname", it will reboot as - no surprises - "secondname".

By default, it will stay as "secondname.local" until the next factory reset. After that it will return again to "firstname" until either another host command changes it, or a new value is compiled in.

In some circumstances, you may want it to always revert to the compiled in name "firstname" even after a `h4/upnp/name` command. The default behaviour described above can be changed by editing `H4PConfig.h` and setting `H4P_PREFER_PERSISTENT` to `false`.

---

# "Tweakables"

The following values are defined in `H4PConfig.h` . They are chosen initally to set a good balance between stability, performance and memory / stack usage. *It is not advisable to change them unless you know exactly what you are doing and why*. 

**N.B.** **Support will not be provided if any of these values are changed.**

* H4P_UDP_JITTER

The number of milliseconds entropy between successive UPNP bradcasts, to minimise queue growth and UDP "flooding".

* H4P_UDP_REFRESH

The number of milliseconds "lifetime" between UPNP "keepalive" broadcasts

* H4P_UDP_REPEAT

The integer number of repeat UDP messages sent on each occasion to prevent packet loss

---

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
