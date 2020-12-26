![H4P Logo](/assets/H4PLogo.png)

# Arduino IDE library targeting ESP8266, ESP32 providing IOT functionality including WiFi, MQTT, Logging, Presence Detection, NTP sync, GPIO management and diagnostic tools

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

[Release Notes v1.0.0](docs/rn100.md)

---
Think of [**H4**](https://github.com/philbowles/H4) and its plugins as "IOT Lego" or an "IOT Swiss Army Knife" (or both) for ESP8266 and ESP32 to get your IOT app up and running in minutes rather than weeks. If you have some basic C / C++ experience then [**H4**](https://github.com/philbowles/H4) and H4Plugins are for you! 

![H4PluginsFF](/assets/h4plugins.jpg)

H4Plugins includes modules for WiFi + OTA, Webserver, MQTT, numerous common types of GPIO handling (e.g. debouncing, rotary encoders), Amazon Alexa voice control, NTP synchronisation, device presence detection, logging to remote MySQL server and extensive diagnostics. By "plugging " together only the required modules, you can rapidly build your own custom firmware or IOT app. Everything you build will be stable and responsive: the plugins work together to allow multiple simultaneous processes to run, so ***no more WDT resets***! As your experience grows you can extend your app / firmware with H4Plugins' well-documented API and runtime command system. 

---

# Contents

* [Getting Started](#getting-started)
* [Examples / Ideas](#examples--applications--ideas)
* [List of Plugins](#plugins)
* [Compatible Hardware](#compatible-hardware)
* [Installation](#installation)

---
# Getting Started

H4 and its plugin system represent a very different way of getting started on ESP8266 from the "standard" examples found all over the web.

There are some very good reasons for that but it means that there are a few things you need to know about H4 and how it works before "diving in" to the H4Plugins system. Spending a few minutes reading [the basics of H4](https://github.com/philbowles/H4#why-do-i-need-it) will save you days and even weeks of effort, so please digest it all before trying some of the 70+ examples in this repo - it will be time well spent!

Once you "get" how H4 and the plugin system work, you will be able to churn out fully working, multi-functional **stable** IOT apps on ESP8266 in only a few lines of code, compared with literally *hundreds* (sometimes even *thousands*!) of lines in the "standard" way of doing things. That's if you can even find anything close to what you need...

So only continue if ( after having read [the basics of H4](https://github.com/philbowles/H4#why-do-i-need-it) ) you now know what *events* are, what *the queue* is and why you need them.

To get a "feel" for just how different H4 and H4Plugins are from all the other examples you will find on the web, let's see an example of H4Plugins being used as replacement firmware for a SONOFF Basic switch.

## SONOFF Basic Example

What follows is the *entire H4Plugins code* - despite the fact it might look like there is something missing, there is not. The code as shown compiles to produce firmware that can be uploaded directly to any / all of your SONOFF devices and provides:

* Autoconnect/reconnect of failed WiFI and/or MQTT with no reboot
* AP mode configuration
* OTA
* Full control via Serial console
* Full control via MQTT including publish on state change
* Full control via built-in Web console
* Single-button multifunction control for on/off, reboot, factory reset and forced AP Mode  depending on the hold time
* LED pattern signalling, e.g. ... --- ... ("SOS") = No WiFi, two short blips = no MQTT, single slow blip = AP mode etc
* HTTP REST control from any other device with JSON response
* Dynamic reconfiguration of SSID and/or MQTT server
* Optional master/slave of other H4/Plugins devices
* UPNP device control from e.g. Windows Network Explorer
* Amazon ECHO "Alexa" voice control

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop
H4P_GPIOManager h4gm;
H4P_FlasherController h4fc;
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","eiffel");
H4P_AsyncMQTT h4mqtt("192.168.1.4",1883);
H4P_AsyncWebServer h4asws;
H4P_BinarySwitch h4onof(RELAY_BUILTIN,ACTIVE_HIGH,OFF);
H4P_UPNPServer h4upnp("Salon Eiffel Tower");
H4P_MultiFunctionButton h4mfb(BUTTON_BUILTIN,INPUT,ACTIVE_LOW,15,LED_BUILTIN,ACTIVE_LOW);
```

As you can see, all you need to do is list the modules/functionality you require (each one is a "plugin") and provide a few necessary values such as ssid / passwords etc and the plugins link up with each other, exchange messages between themselves and "stitch" everything together into a seamless piece of stable firmware. If you know of anything easier for both beginners and seasoned developers, please let us know.

## Designed for programmers

The modular design of H4's plugin architecture minimises scarce resources in low-memory MCU targets: You only compile in what you need by choosing the relevant bulding blocks. Detailed logging and/or diagnostics can be easily included (or completely compiled-out) and controlled at runtime via the serial console, web console,HTTP REST or MQTT depending on which options you choose. It is built on top of the very stable [H4](https://github.com/philbowles/H4) timer/scheduler which traces its ancestry back to "Esparto" - of which one user recently said: *"and now have Esparto modules with months of uptime without an issue"*.

There are over 70 example sketches demonstrating all the features and the API of all of the plugins. They should be used both as a template for your own sketches and as a learning resource.

Each plugin is also throroughly documented in the links below. *Please make sure you have read and fully understood the documentation for the [H4 library](https://github.com/philbowles/H4) and the relevant Plugin(s) before raising an issue.*

Also please prefer the [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/) group over the github issues mechanism, as I don't look at github that often, nor open my email until it backs up, but I'm on FB pretty much every day and you will get a much quicker response.

## The "Common Command Core" concept

Take a simple device that switches on a relay, perhaps connected to a lamp. Its most fundamental control mechanism is to be switched either ON or OFF.

H4Plugins allows that switching to be done in a number of ways, including:

* Physical GPIO switch
* MQTT message
* Web User Interface
* Serial Console
* Remote HTTP REST-like interface
* Internal program code
* Amazon Alexa Voice command

(Depending on which plugins you choose to include in your app)

H4/Plugins tries to make the format of all those commands as close as possible to being the same and one single, common piece of code is called no matter from which source the event was initiated.

For example, imagine our device is called demo and is on IP 192.168.1.4

* If I am connected to it via a serial console, to turn it on I would type:

`h4/on`

* If I have an MQTT client, I would publish the topic:

`demo/h4/on`

* If I have some other code such as NODE-RED running, I can send an http:// request:

`http://192.168.1.4/rest/h4/on`

* If I click the "Big red button" on the web user interface, the javascript actually sends an ajax request to itself:

`http://192.168.1.4/rest/h4/on`

* I can shout "Alexa! Switch on demo" and the same code that gets run by any / all of the above will execute and turn on the relay -> lamp.

In summary, a command is a command is a command no matter where it comes from. Subject to the necessary quirks of each source, the final part of the command is always the same. This means that when reading the documentation, only that last part of the command described, no matter how simple or how complex its effect is.

If it can be done by *any* of the methods described above, it can - by definition - be done by *all* of them. So in the documentation or support groups you might see "try giving it an h4/reboot" or "what is the result of an h4/show/config?" and how you actually do that to *your* device depends entirely on what plugins you have included: in effect the actual command itself is disconnected from its source - how you choose to get the command into the device is up to you, but the device will do exactly the same thing whichever method you choose.

---

# Examples / applications / ideas

## Web console, on/off control on mobile phone:

![SONOFF](/assets/webui056.jpg)

## Windows 10 Network Explorer interface

![UPNP](/assets/upnpsmall.jpg)

## Extensive GPIO library

Simple one-line GPIO management of debouncing, retriggering, pulse counting, timing rotary encoders etc

![UPNP](/assets/GPIOsmall.jpg)

## Cross-platform code

Some of the plugins (non WiFi-dependent) also run on STM32-NUCLEO boards. Some have been tested also on Raspberry Pi and even Ubuntu linux. 
***N.B. While this may still be true in many cases, STM32 support has been withdrawn as at v1.0.0***

![UPNP](/assets/xplat.jpg)

## Device drivers

H4Plugins make it really easy and quick to write your own safe, stable fimrware for novel devices. The code co-operates automatically with (and can be controlled by) existing plugins. As an example H4Plugins comes with a fully working controller for a cheap ($3) square-wave generator which provides both frequency and duty cycle sweeps as well as simple F and DC value setting via Serial, HTTP REST and/or MQTT

When you think that H4Plugins also has "plug and play" rotary encoder handling, it is easy to see how you could build a complete WiFi and manual-control lab instrument in a day. *I* did!

![SQWV](/assets/sqwvhardware.jpg)

---

# Plugins
## Core IOT functionality

* [**H4P_AsyncMQTT**](docs/h4mqtt.md): Automatic Connection/ reconnection MQTT client (allows remote control of H4)
* [**H4P_AsyncWebServer**](docs/h4asws.md): Fully Asynchronous Webserver  + HTTP REST handler + user fields manager
* [**H4P_BinarySwitch**](docs/things.md): GPIO object that allows control by commands from multiple sources
* [**H4P_BinaryThing**](docs/things.md): user-defined functional object that allows control by commands from multiple sources
* [**H4P_ConditionalSwitch**](docs/things.md): H4P_BinarySwitch that only operates when user-define function returns true
* [**H4P_ConditionalThing**](docs/things.md): H4P_BinaryThing that only operates when user-define function returns true
* [**H4P_FlasherController**](docs/h4fc.md): One-line coding of multiple simultaneous LED flashing by Square Wave, PWM, abitrary pattern and Morse code
* [**H4P_GPIOManager**](docs/h4gm.md): One-line coding of debouncing, retriggering, rotary encoding plus numerous other GPIO strategies
* [**H4P_H4Detector**](docs/h4pd.md): Execute function when specific H4 device joins / leaves network
* [**H4P_H4DetectorSource**](docs/h4pd.md): Switch default BinarySwitch/Thing when specific H4 device joins / leaves network
* [**H4P_IPDetector**](docs/h4pd.md): Execute function when specific IP address joins / leaves network
* [**H4P_IPDetectorSource**](docs/h4pd.md): Switch default BinarySwitch/Thing when specific IP address joins / leaves network
* [**H4P_MDNSDetector**](docs/h4pd.md): Execute function when specific MDNS service/protocol found / lost on network
* [**H4P_MDNSDetectorSource**](docs/h4pd.md): Switch default BinarySwitch/Thing when specific MDNS service/protocol found / lost on network
* [**H4P_MultiFunctionButton**](docs/h4mfnb.md): Tact-button giving on/off, reboot,factory reset and forced AP Mode depending on hold time
* [**H4P_PersistentStorage**](docs/h4stor.md): Save name/value pairs across reboots (requires LittleFS)
* [**H4P_RemoteUpdate**](docs/h4ru.md): OTA update from remote server
* [**H4P_SerialCmd**](docs/h4cmd.md): Send commands from multiple sources to H4 and/or plugins to control and/or diagnose
* [**H4P_Sunrise**](docs/h4tk.md#sunrisesunset) **EXPERIMENTAL** : Gets sunsrise / sunset times from public API give Lat/Long
* [**H4P_Timekeeper**](docs/h4tk.md): NTP server sync for "clock time" alarms and scheduling
* [**H4P_ToneController**](docs/h4tc.md) Buzzer, tones, sirens and...music!
* [**H4P_UPNPDetector**](docs/h4pd.md): Execute function when specific UPNP device USN joins / leaves network
* [**H4P_UPNPDetectorSource**](docs/h4pd.md): Switch default BinarySwitch/Thing when specific UPNP device USN joins / leaves network
* [**H4P_UPNPServer**](docs/things.md): provides full UPNP device with Alexa voice control, Windows10 Desktop integration
* [**H4P_WiFi**](docs/h4wifi.md): Automatic Connection / reconnection manager + AP configuration + OTA
  
## Diagnostic / Logging / Development tools:

* [**H4P_CmdErrors**](docs/h4ce.md): Provide text error messages instead of error codes to SerialCmd
* [**H4P_HeapWarn**](docs/h4hw.md): Call user function on low Heap
* [**H4P_Heartbeat**](docs/advanced.md): call user-defined function exry 1 second
* [**H4P_HttpMySQLLogger**](docs/mysql.md): log to remote webserver to update MySQL log db (server example provided)
* [**H4P_LoopCount**](docs/h4qw.md): Display loops/sec for diagnosing bottlencks
* [**H4P_LocalLogger**](docs/h4logs.md): Event logging to LittleFS file
* [**H4P_MQTTHeapLogger**](docs/h4logs.md): Specialised H4P_MQTTLogger which periodically logs value of free heap
* [**H4P_MQTTLogger**](docs/h4logs.md): Event logging to MQTT Server
* [**H4P_MQTTQueueLogger**](docs/h4logs.md): Specialised H4P_MQTTLogger which periodically logs size of Queue
* [**H4P_QueueWarn**](docs/h4qw.md): Call user function on low Queue
* [**H4P_SerialLogger**](docs/h4logs.md): Event logging to serial monitor
* [**H4P_TaskSniffer**](docs/h4ts.md): Low-level task / queue dumper for H4 + Plugins

## Specialist Device Drivers

* [**H4P_ExternalSqWave**](docs/h4esw.md): Serial driver for cheap ebay square wave device

![sqwv](assets/sqwvhardware.jpg)

---

# Compatible Hardware

* ESP8266 - any board e.g. nodeMCU, Wemos D1 Mini etc
* ESP8285 - any board e.g. Wemos D1 mini Lite
* ESP32 - any board e.g. DevKit, Lolin D32
* SONOFF Basic, S20, SV others may work too if ESP8266 based

---

# Installation

H4Plugins is tested using

* H4 1.0.0
* ArduinoIDE 1.8.13
* ESP8266 core 2.7.4
* ESP32 core 1.0.4

Use the ArduinoIDE boards manager to install those versions. Earlier version *may* work, but I am only able to offer support when all the above are in use.

H4Plugins is a standard Arduino library. The simplest method is to download the zip form the link above and then use the menu command: `Sketch / Include Library / Add .ZIP Library...`

First you need to install the [H4](https://github.com/philbowles/H4) library. 

Next install the 3rd-party libraries:

* [PangolinMQTT](http://github.com/philbowles/PangolinMQTT)
* [ESP8266 ESPAsyncUDP Library](https://github.com/me-no-dev/ESPAsyncUDP)
* [ESP32 AsyncTCP Library](https://github.com/me-no-dev/AsyncTCP)

The above libraries coexist quite happily if you download all of them to enable targetting both ESP8266 and ESP32.

* [ESPAsyncWebServer](https://github.com/philbowles/ESPAsyncWebServer)
* [ESP8266 ESPAsyncTCP Library](https://github.com/philbowles/ESPAsyncTCP)

**N.B** If you have previously installed the original version of either of the above, by "me-no-dev" you *must* uninstall it and use only the patched version. 
The author seems unwilling/unable to fix several serious bugs - the above version work *and are required* for correct functioning of H4/Plugins.
  
* Finally, install this H4Plugins library

If using WiFi, you will need to install either the [LittleFS upload tool](https://github.com/earlephilhower/arduino-esp8266littlefs-plugin) or the [ESP32 sketch data uploader](https://github.com/me-no-dev/arduino-esp32fs-plugin) (or both) depending on which platform you compile for. 

## Tools / Build Guidelines

To reduce the footprint of the binary code and provide best WiFi / MQTT performance, the following IDE options
are recommended (if available for the chosen board):

## For ALL sketches

![Settings](assets/settings.jpg
)
### **IMPORTANT**

WiFi sketches must reserve LittleFS space to hold the configuration web UI pages. These take < 64K so allocate the minimum LittleFS possible that is greater than or equal to 64k and remember to copy the `data` folder to the sketch folder and upload using either the  [LittleFS upload tool](https://github.com/earlephilhower/arduino-esp8266littlefs-plugin) or the [ESP32 sketch data uploader](https://github.com/me-no-dev/arduino-esp32fs-plugin) (or both) depending on which platform you compile for.

## H4 Optimised Board Definitions

See [Advanced Topics](docs/advanced.md) for how to simply add H4 optimised board definitions that will ensure you get the smallest possible binary (and therefore most likely to be OTA-able)

![Optimised Boards](assets/optismall.jpg)

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
