![H4P Flyer](/assets/GPIOLogo.jpg) 

# Release Notes v0.5.1 31st March 2020

## Introduction

This is a major release, there have been significant changes so it is important to read this in its entirety.

## Updated H4 prerequisite

User must first upgrade the H4 library to version v0.5.1

## New

### [Presence detection](h4pd.md)
  
The main addition is "presence detection" - the ability to react to devices joining or leaving the networl. This is useful for example to switch on outisde lights as you approach your home and your mobile phone comes into range of your WiFi router.

Methods are provided for detection of

* IP addresses
* MDNS service/protocol
* H4 devices
* UPNP USN identifiers

Each of these has a version that linkes directly to a [H4P_BinarySwitch / H4P_BinaryThing](things.md)

* Master / slave linkage

H4 devices can be commanded to link to other devices such that turning on the master will automatically turn on all slaves.

The new command is `h4/slave/< name of H4 slave device >` with a payload of 1 to add (link to) the slave, zero to remove (unlink) the slave.

---

### Commands

	* `h4/apmode` force device into AP mode (see below)
    * `h4/host2/D,U` configures host device name D *and* upnp friendly name U in one command
    * `h4/show/heap` a useful diagnostic feature
	* `h4/slave/S` (S = another H4: see above)

The `host2` is useful when using a "generic" sketch to be loaded into multiple devices without hvaing to recompile individually for each device. The device host name and UPNP friendly name are not provided in the sketch, but sent via e.g. MQTT commands on startup.

Since the `host` and `upnp` comand each cause a reboot, using a single `host2` to do both means the dvice onlyreboots once after it ha been remotely configured.

Two new example sketches are provided in the `examples/xtras` folder showing how this is easily achieved.

---

### Functions

* `h4ce.getTaskType` translates numeric task types into human-readable form
* `h4ce.getTaskName` translates numeric task names into human-readable form
  
---

### Additional Example sketches

    * [Remote configurator for generic sketches](../examples/xtras/ChezToiioT_Config/ChezToiioT_Config.ino)
    * [Generic SONOFF sketch](../examples/xtras/Generic_SONOFF/Generic_SONOFF.ino)
    * [Presence detection - H4 Device](../examples/H4P_PresenceDetectH4/H4P_PresenceDetectH4.ino)
    * [Presence detection - IP](../examples/H4P_PresenceDetectIP/H4P_PresenceDetectIP.ino)
    * [Presence detection - MDNS service/protocol](../examples/H4P_PresenceDetectMDNS/H4P_PresenceDetectMDNS.ino)
    * [Presence detection - UPNP USN identifier](../examples/H4P_PresenceDetectUPNP/H4P_PresenceDetectUPNP.ino)
    * [H4P_BinaryThing auto-off feature](../examples/H4P_BinaryThingAutoOff/H4P_BinaryThingAutoOff.ino)

---

## Changed

### Mandatory starting requirements

The standard opening of any H4 sketch has changed considerably. First the mandatory call to H4 has been moved into `H4_USE_PLUGINS` so it is no longer required. Also, a decision was made to incorporate H4P_SerialCmd into all sketches automatically so this has also been moved into `H4_USE_PLUGINS` which now performs all sketch initialisation in a single line.

This means that the parameters to H4 *and* H4P_SerialCmd are now passed instead to `H4_USE_PLUGINS`. A typical sketch now starts:

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop
```

The parameters are:

* Serial baud rate. If No Serial output is required, use 0
* Queue size. There is little reason to change this from the default 20, and only expert users should change this.
* New is the "`autoStop`" feature of H4P_SerialCmd

Stopping the `scmd` service improves performance by up to 50% so should be considered for all production code. The downsdie is that you are no longer able to issue Serial console commands. This can be reversed by remote restart of the scmd service (e.g. MQTT, HTTP REST).

The default is false for development and testing, i.e do NOT auto-stop the `scmd` service and allow Serial console commands, at the expense of overall performance.

Finally, the "handle" to the SerialCmd service has been changed from `h4sc` to `h4cmd`

---

### ALL Plugins as services

Previously, some plugins such as WiFi and MQTT ran as "services" meaning they could be started nd stopped and othersiwe controlled in a standard manner. This has been extended to *all* plugins so that there is a standard interface for everything and far fewer individual commands.

This means you *must* read the section on [command handling](h4cmd.md) before digging deeper into v0.5.1

---

### ["Things and switches"](things.md)

The naming of the input connectors to drive Things or switches was confusing and they have now been changed to end in "Source". Any plugin ending this way will automatically link to either an `H4P_BinarySwitch` or `H4P_BinaryThing`. Also, the final parametr of all thes calls has been removed, as they each connect automatically to the  default output device (`h4onof`).

Additionally, there is now an "autoOff" parameter on both `H4P_BinarySwitch` and `H4P_BinaryThing` meaning that they will turn themselves off after a given amount of time, irrespective of which source commanded them on. If left at the default valueof 0 they will stay one until commanded off.

The UPNP variants of Switch and Thing have been removed and replaced by a single `H4P_UPNPServer` which acts as a source in the same way as the GPIO input connectors.

In summary, a sketch may now have only a `H4P_BinarySwitch` or `H4P_BinaryThing` as its default action. This can be driven by any of the following input sources:

* GPIO-Related:

    * AnalogThresholdSource
    * DebouncedSource
    * EncoderSource
    * LatchingSource
    * PolledSource
    * RawSource
    * RetriggeringSource
    * MultiFunctionButton

* Network-related

    * MQTT commands on/off/toggle/switch
    * HTTP REST requests on/off/toggle/switch
    * Presence detection of:
        * IP Address
        * H4 Device
        * UPNP USN identifier
        * MDNS service/protocol

* Serial console commands on/off/toggle/switch

![BT vs BS](../assets/switchthing.jpg)

---

### AP Mode

A decision was taken to remove any automatic start into AP Mode, as used to happen after a factory reset.

The rationale is that H4/Plugins is a programming framework, not a "drop-in" replacement, so you wull always be starting from the point of compiling your ssid and password into your code.

There is still the ability to force AP mode (for whatever reason) using the H4P_MultifunctionButton. This was previously the H4P_ThreeFunctionButton and would initiate a factory reset after beinf held for more than 5 seconds.

The H4P_MultifunctionButton is now a *four*-function button. If held for more than 10 seconds it will force a reboot into AP mode. Also you can use the `h4/wifi/change/newssid,newpassword` command to change the ssid "on the fly".

Given these options, it was felt that there was no strong reason for ever booting into AP mode.

---

### Renamed commands

    * `h4/mqtt/change` (change MQTT server:port) is now `h4/mqtt`

    * `h4/upnp/name` (change UPNP device friendly name) is now `h4/upnp`

---

### MQTT library changed

H4?plugins now use the [MQTT Async Client library]() in place of `PubSubClient`. You must donwload and install the Async lib before compiling any v0.5.1 programs

---

## REMOVED

### Unloading

All refernces, functions, commands related to unloading plugins have been removed, as the same functionality is now provided by the service stop commands

### Trusted names

All refernces, functions, commands related to "trusted names" have been removed, as the same functionality is now provided by service names an the human-readable values for these is no available in the new functions `h4ce.getTaskType` and `h4ce.getTaskName`

---

(c) 2020 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Blog](https://8266iot.blogspot.com)
* [Facebook Esparto Support / Discussion](https://www.facebook.com/groups/esparto8266/)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook IOT with ESP8266 (moderator)](https://www.facebook.com/groups/1591467384241011/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)
