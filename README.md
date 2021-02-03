![H4PluginsFF](/assets/h4plugins.jpg)

# ESP8266/ESP32 ArduinoIDE library of IOT functions including WiFi, MQTT, OTA updates, NTP, UPNP, SSDP, Alexa voice control and much more...

[Release Notes v1.0.1](docs/rn100.md) This is a *major* release, existing users *must* read the release notes as they will need to make changes to existing code. As an *absolute minimum* you will need to update [H4](https://github.com/philbowles/H4), [PangolinMQTT](http://github.com/philbowles/PangolinMQTT) and [ESPAsyncWebServer](https://github.com/philbowles/ESPAsyncWebServer) libraries.

***N.B. The code is tested and stable but parts of the documentation are still being updated. Until this message is removed , refer to the 70+ example sketches for detailed API usage. This document itself is accurate and up-to-date as at 3/2/2021***

---

# Contents

* [Introduction](#introduction)
* [Diving In](#diving-in)
* [List of Plugins](#plugins)
* [Compatible Hardware](#compatible-hardware)
* [Installation](#installation)
* [Documentation Master Index](docs/index.md)

---

# Introduction

Think of [H4](https://github.com/philbowles/H4) and its plugins as "IOT Lego" or an "IOT Swiss Army Knife" (or both) to get your IOT app up and running in minutes rather than weeks. If you have some basic C / C++ experience then [H4](https://github.com/philbowles/H4) and H4Plugins are for you! 


H4Plugins includes modules for WiFi + OTA, Webserver, MQTT, numerous common types of GPIO handling (e.g. debouncing, rotary encoders), Amazon Alexa voice control, NTP synchronisation, device presence detection, logging to remote MySQL server and extensive diagnostics. By "plugging " together only the required modules, you can rapidly build your own custom firmware or IOT app. Everything you build will be stable and responsive: the plugins work together to allow multiple simultaneous processes to run, so ***no more WDT resets***! As your experience grows you can extend your app / firmware with H4Plugins' well-documented API and runtime command system.

---

# Diving in

[H4](https://github.com/philbowles/H4) and its plugin system represent a very different way of getting started on ESP8266 from the "standard" examples found all over the web.

There are some very good reasons for that but it means that there are a few things you need to know about [H4](https://github.com/philbowles/H4) and how it works before "diving in" to the H4Plugins system. Spending a few minutes reading [the basics of H4](https://github.com/philbowles/H4#why-do-i-need-it) will save you days and even weeks of effort, so please digest it all before trying some of the 70+ examples in this repo - it will be time well spent!

Once you "get" how H4 and the plugin system work, you will be able to churn out fully working, multi-functional **stable** IOT apps on ESP8266 in only a few lines of code, compared with literally *hundreds* of lines in the "standard" way of doing things. That's if you can even find anything close to what you need...

So only continue if ( after having read [the basics of H4](https://github.com/philbowles/H4#why-do-i-need-it) ) you now know what *events* are, what *the queue* is and why you need them. To get a "feel" for just how different H4 and H4Plugins are from all the other examples you will find on the web, let's see an example of H4Plugins being used as replacement firmware for a SONOFF Basic switch.

![sonoff](assets/sonoff.jpg)

## SONOFF Basic Example

What follows is the *entire H4Plugins code* - despite the fact it might look like there is something missing, there is not. The code as shown compiles to produce firmware that can be uploaded directly to any / all of your SONOFF devices and provides:

* Autoconnect/reconnect of failed WiFI and/or MQTT with no reboot
* AP mode configuration
* OTA
* Full control via Serial console
* Full control via MQTT plus publishing of state change
* Full control via built-in Web console
* Single-button control for on/off, reboot, factory reset depending on the time pressed down
* LED pattern signalling, e.g. ... --- ... ("SOS") = No WiFi, two short blips = no MQTT, single slow blip = AP mode etc
* HTTP REST control from any other device with JSON response
* Dynamic reconfiguration of SSID and/or MQTT server
* Optional master/slave of other H4/Plugins devices
* UPNP device control from e.g. Windows Network Explorer
* Amazon ECHO "Alexa" voice control

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,10,false) // Serial baud rate, Q size, SerialCmd autostop
H4P_WiFi h4wifi("XXXXXXXX","XXXXXXXX","salon");
H4P_AsyncMQTT h4mqtt("192.168.1.4",1883);
H4P_BinarySwitch h4onof(RELAY_BUILTIN,ACTIVE_HIGH,OFF);
H4P_UPNPServer h4upnp("Salon Corner Lamp");
H4P_MultiFunctionButton h4mfb(BUTTON_BUILTIN,INPUT,ACTIVE_LOW,15);
```

And here's what you see when you point your browser to the webserver running inside the device:

![sonoffbasic](/assets/sonoffbasic.jpg)

The "big red button" switches the device on / off, but you can also do it using MQTT, HTTP, serial monitor, Alexa voice command an *even* the physical button if you simply have to.

You alse get a "web console" that allows yout to control the device in the same way as if you were connected via a Serial link. It is also visible to your PC's Network Explorer: double click jumps you straight to the builtin webserver seen above.

![sonoffbasic](/assets/upnp2.jpg)

As you can see (from the 7 lines of code :smile: ), pretty much all you need to do is list the modules/functionality you require (each one is a "plugin") and provide a few necessary values such as ssid / passwords etc. The plugins link up with each other, exchange messages between themselves and "stitch" everything together into a seamless piece of stable firmware. If you know of anything easier, please telll us!

## Designed for programmers

The modular design of H4's plugin architecture minimises scarce resources in low-memory MCU targets: You only compile in what you need by choosing the relevant bulding blocks. Detailed logging and/or diagnostics can be easily included (or completely compiled-out) and controlled at runtime via the serial console, web console,HTTP REST or MQTT depending on which options you choose. It is built on top of the very stable [H4](https://github.com/philbowles/H4) timer/scheduler which traces its ancestry back to "Esparto" - of which one user recently said: *"and now have Esparto modules with months of uptime without an issue"*.

There are over 70 example sketches demonstrating all the features and the API of all of the plugins. They should be used both as a template for your own sketches and as a learning resource.

Each plugin is also throroughly documented in the links below. *Please make sure you have read and fully understood the documentation for the [H4 library](https://github.com/philbowles/H4) and the relevant Plugin(s) before raising an issue.*

Also please prefer the [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/) group over the github issues mechanism, as I don't look at github that often, nor open my email until it backs up, but I'm on FB pretty much every day and you will get a much quicker response.

---

# Plugins

## Basics & GPIO (no network required)

* [H4P_SerialCmd](docs/h4cmd.md): Send commands to control and/or diagnose all other plugins. This is so important, it is automatically included for you and its the first plugin you should get to know.

* [H4P_FlasherController](docs/h4fc.md): One-line coding of multiple simultaneous LED flashing by Square Wave, PWM, abitrary pattern and Morse code

* [H4P_GPIOManager](docs/h4gm.md): One-line coding / mangement of a variety of different GPIO strategies, e.g. debouncing, decoding of rotary encoders, etc - there are NN in all to choose from to match pretty much any kind of hardware you might want attach to the GPIO.

* [H4P_ToneController](docs/h4tc.md) Buzzer, tones, sirens and...yes, actual music! Can play simultaneous different "voices" (musical parts) on multiple buzzers.

* [H4P_PersistentStorage](docs/h4stor.md): Save name/value pairs across reboots (requires LittleFS)

## Switches and "Things"

Many iot devices have one simple thing to do: switch something on/off on a single GPIO - like a relay for example. If that's all you need, use a plugin with "...Switch" on the end of its name. If you need something a little more fancy, for example to do some other "work" then choose a plugin with "...Thing" on the end of its name. The actual "thing" is a function *you* write so it can be anything, but whatever it is, sending `h4/on` or saying "Alexa...switch on 'thing'" will make it do its thing and `h4/off` will stop it.

Networking is not required but if you *are* connected, the webserver will automatically add the "big red switch" and various other options to control the plugin. 

* [H4P_BinarySwitch](docs/things.md): Allows control by commands from multiple sources to on/off a single GPIO. 

* [H4P_BinaryThing](docs/things.md): user-defined function that can be controlled by commands from multiple sources

* [H4P_ConditionalSwitch](docs/things.md): H4P_BinarySwitch that only operates when user-define function returns true

* [H4P_ConditionalThing](docs/things.md): H4P_BinaryThing that only operates when user-define function returns true
## WiFi and networking

* [H4P_WiFi](docs/h4wifi.md): Automatic Connection / reconnection manager, builtin webserver, AP configuration, OTA programming. Also uses LED_BUILTIN to signal network state via various flashing patterns.

* [H4P_AsyncMQTT](docs/h4mqtt.md): Automatic Connection to MQTT server, with reconnection on failure (without reboot!) allows remote control of H4 as well as the more usual sensing og e.g. sensor data to "the cloud"

* [H4P_UPNPServer](docs/things.md): provides full UPNP/SSDP device with Alexa voice control, PC Desktop OS integration

* [H4P_RemoteUpdate](docs/h4ru.md): OTA update from remote server

* [H4P_MultiFunctionButton](docs/h4mfnb.md): Tact-button giving on/off, reboot, factory reset depending on hold time.

* [H4P_Heartbeat](docs/advanced.md): Adds an "Up Time" to the web UI

### Network time, calendar scheduling
* [H4P_Timekeeper](docs/h4tk.md): NTP server sync for "clock time" alarms and scheduling

* [H4P_Sunrise](docs/h4tk.md#sunrisesunset) **EXPERIMENTAL** : Gets sunrise / sunset times from public API given Lat/Long

### Network presence detection

* [H4P_IPDetector](docs/h4pd.md): Execute function when specific IP address joins / leaves network

* [H4P_IPDetectorSource](docs/h4pd.md): Switch default BinarySwitch/Thing when specific IP address joins / leaves network

* [H4P_MDNSDetector](docs/h4pd.md): Execute function when specific MDNS service/protocol found / lost on network

* [H4P_MDNSDetectorSource](docs/h4pd.md): Switch default BinarySwitch/Thing when specific MDNS service/protocol found / lost on network

* [H4P_UPNPDetector](docs/h4pd.md): Execute function when specific UPNP device USN joins / leaves network

* [H4P_UPNPDetectorSource](docs/h4pd.md): Switch default BinarySwitch/Thing when specific UPNP device joins / leaves network
  
## Diagnostic / Logging / Development tools:

* [H4P_VerboseMessages](docs/vm.md): Provide text error messages instead of numeric system codes
  
* [H4P_HeapWarn](docs/h4hw.md): Call user function on low Heap

* [H4P_QueueWarn](docs/h4qw.md): Call user function on low Queue

* [H4P_TaskSniffer](docs/h4ts.md): Low-level task / queue dumper for H4 + Plugins

### Event Listeners

* [H4P_SerialLogger](docs/slog.md): Event logging to serial monitor

* [H4P_LocalLogger](docs/llog.md): Event logging to LittleFS file

* [H4P_MQTTLogger](docs/mlog.md): Event logging to MQTT Server

* [H4P_HttpMySQLLogger](docs/mysql.md): log to remote webserver to update MySQL log db - Includes [example Node.js server](examples/LOGGING/H4P_HttpMySQLLogger/mysqlrest/mysqllogger.code-workspace)

* [H4P_EventListener](docs/ears.md): General-purpose listener calls user-defined function on any selection of events

### Event Emitters

* [H4P_EmitLoopCount](docs/h4qw.md): Send loops/sec to the event queue for diagnosing bottlencks (requires changes to [H4 config](https://github.com/philbowles/H4/blob/src/config.h) / compile options)
  
* [H4P_EmitQ](docs/events.md): Periodically Send H4 Q size to event queue to help profling / performance tuning

* [H4P_EmitHeap](docs/heap.md): Periodically Send `getFreeHeap()` value to event queue to help hunt memory leaks

* [H4P_EmitTick](docs/events.md): Send synchronous 1x per second on-the-second heartbeat event.

---

# Compatible Hardware

* ESP8266 - any board e.g. nodeMCU, Wemos D1 Mini etc
* ESP8285 - any board e.g. Wemos D1 mini Lite
* ESP32 - any board e.g. DevKit, Lolin D32
* SONOFF Basic, S20, SV others may work too if ESP8266 based

---

# Installation

H4Plugins is tested using

* ArduinoIDE 1.8.13
* ESP8266 core 2.7.4
* ESP32 core 1.0.4

Earlier version *may* work, but I am only able to offer support when all the above are in use.

H4Plugins is a standard Arduino library. The simplest method is to download the zip frm the links below and then use the menu command: `Sketch / Include Library / Add .ZIP Library...`

First you need to install the [H4](https://github.com/philbowles/H4) library. 

Next install the 3rd-party libraries:

* [PangolinMQTT](http://github.com/philbowles/PangolinMQTT)
* [ESP8266 ESPAsyncUDP Library](https://github.com/me-no-dev/ESPAsyncUDP)
* [ESP32 AsyncTCP Library](https://github.com/me-no-dev/AsyncTCP)

The above libraries coexist quite happily if you download all of them to enable targetting both ESP8266 and ESP32. 

Also, install the patched versions of these 3rd-party libraries:

* [ESPAsyncWebServer](https://github.com/philbowles/ESPAsyncWebServer)
* [ESP8266 ESPAsyncTCP Library](https://github.com/philbowles/ESPAsyncTCP)

**N.B** If you have previously installed the original version of either of the above, by "me-no-dev" you *must* replace it with the patched version. The originals of these are buggy and will prevent H4/Plugins from working correctly, so the patched versions *are required*.
  
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
# What next? 

There is *a lot* in this library and its pre-requisite: the [H4](https://github.com/philbowles/H4) library. For a start, there are over 30 individual plugins, as well as several new concepts to learn, although you will probably only use 6 or 7 of the more common / important ones for most of your apps.

Then there are over 80 example sketches in the two libraries. That's not a typo: it's "over eighty". Every plugin and every possible API call is demonstrated at least once somewhere in that collection. Its designed to help you get up to speed quickly - use it! 90% of the time you will find the start of the app you want to write.

After you have finished reading this file, the recommended path you should follow for mastery of the library is:
## Step-by-step guide to mastery

1. Download, install and play with [H4](https://github.com/philbowles/H4). Pick one or two of the examples and run it. There is no better way to learn that actually *doing it*

2. Read the [H4](https://github.com/philbowles/H4) documentation *fully*. Everything H4 does is based on using the features of [H4](https://github.com/philbowles/H4). All of the examples here use the [H4](https://github.com/philbowles/H4) API extensively. If you don't "get" [H4](https://github.com/philbowles/H4) you will certainly not be able to use H4Plugins to its best. Next, make some small changes to one or two of the examples, or be brave and write your own

3. By this point you will have got "the feel" of event-driven programming and how *everything* is done in H4Plugins.

4. Read [Common Command and Control: H4P_SerialCmd](docs/ccc.md) as it serves as the basis for dealing with all plugins and all apps you create.
   
5. Read [Switches and Things](docs/things.md) as the vast majority of simple apps will have one of these as is main function...
   
6. ...such as the [basic sonoff app](examples/XTRAS/H4P_SONOFF_Basic/H4P_SONOFF_Basic.ino) (it works on pretty much any hardware) Compile it and play with controlling it via serial, builtin webserver, mqtt , rest etc
   
7. Read [Event Listeners and Event Emitters](docs/h4logs.md) because in this class are some incredibly useful tools for helping you debug your code and log any problems or errors.
   
8. Now work your way through several of the [GPIO examples](examples/GPIO). Start with ["raw"](examples/GPIO/H4GM_Raw/H4GM_Raw.ino) and then ["debounced"](examples/GPIO/H4GM_Debounced/H4GM_Debounced.ino) - once you understand those two all the rest will start to make a lot more sense. If you DO have a [rotary encoder](examples/GPIO/H4GM_Encoder/H4GM_Encoder.ino), hook it up and delight in the ease with which you will be able to use it!
   
9.  Try a few of the other examples to give yourself an idea of the range and flexibility of the whole system
    
10. Read the [Advanced Topics](docs/advanced.md) document for how to set up and deploy an entire self-updating cloud-free framework controlled by NODE-RED

---

# Problems / Issues

H4 / H4Plugins are designed to be easy to use, but the price of that is that the project is large and complex "under the hood". Its flexibility means that there are as many ways to do things as there are different programming styles. Because my resources are limited, I must insist that all reports provide at least enough information for me to replicate the problem. What you need to provide is described fully in [Raising an Issue](docs/issues.md)

In terms of speed of response, you are *far* more likely to get a rapid answer if you "ping" me on one of the social media links below: I'm on those every day at some point, I may only visit GitHub once a week or so.
 # N.B. No problem report / issue will be considered or dealt with unless it complies FULLY with the requires laid out in [Raising an Issue](docs/issues.md) . 
 
 #### Help me to help YOU by provding all the information required in the relevant format

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)