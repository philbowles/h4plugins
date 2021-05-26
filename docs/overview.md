![H4Plugins](../assets/H4PLogo.png)

# Plugins Overview
---

A brief introduction to the range of functions available. Each Plugin has its own formal API document and is usually introduced and desctribed as part of more general discussion document, which should of course be read before the API document

---

# Contents

The Plugins are described in broadly functional areas:

* [Basics & GPIO](#basics--gpio-no-network-required)
* [Switches and "Things"](#switches-and-things)
* [WiFi and networking](#wifi-and-networking)
* [Diagnostics, Logging and development](#diagnostic--logging--development-tools)
* [Documentation Master Index](docs/index.md)

---

# Basics & GPIO (no network required)

* [H4P_SerialCmd](docs/h4p.md): Send commands to control and/or diagnose all other plugins. This is so important, it is automatically included for you and its the first plugin you should get to know.

* [H4P_Signaller](docs/h4fc.md): One-line coding of multiple simultaneous LED flashing by Square Wave, PWM, abitrary pattern and Morse code

* [H4P_PinMachine](docs/h4gm.md): One-line coding / mangement of a variety of different GPIO strategies, e.g. debouncing, decoding of rotary encoders, etc - there are over 20 covering pretty much any kind of hardware you might want attach to the GPIO pins.

* [H4P_ToneController](docs/h4tc.md) Buzzer, tones, sirens and...yes, actual music! Can play simultaneous different "voices" (musical parts) and/or chords on multiple buzzers.

* [H4P_Voice](h4pvox.md) Single musical part or "stave" - sends a stream of tones to one single buzzer. Used by [H4P_ToneController](docs/h4tc.md) to play chords, multi-part tunes

---

# Switches and "Things"

Many iot devices have one simple thing to do: switch something on/off on a single GPIO - like a relay for example. If that's all you need, use a plugin with "...Switch" on the end of its name. If you need something a little more fancy, for example to do some other "work" then choose a plugin with "...Thing" on the end of its name. The actual "thing" is a function *you* write so it can be anything, but whatever it is, sending `h4/on` or saying "Alexa...switch on 'thing'" will make it "do its thing" and `h4/off` will stop it.

Networking is not required but if you *are* connected, the webserver will automatically add the "big red switch" and various other options to control the plugin. 

* [H4P_BinarySwitch](docs/things.md): Allows control by commands from multiple sources to on/off a single GPIO. 

* [H4P_BinaryThing](docs/things.md): user-defined function that can be controlled by commands from multiple sources

* [H4P_ConditionalSwitch](docs/things.md): H4P_BinarySwitch that only operates when user-define function returns true

* [H4P_ConditionalThing](docs/things.md): H4P_BinaryThing that only operates when user-define function returns true
  
---

# WiFi and networking

* [H4P_WiFi](docs/h4wifi.md): Automatic Connection / reconnection manager, builtin webserver, AP configuration*, OTA programming. Also uses LED_BUILTIN to signal network state via various flashing patterns.

* [H4P_AsyncMQTT](docs/h4mqtt.md): Automatic Connection to MQTT server, with reconnection on failure (without reboot!) allows remote control of H4 as well as the more usual sensing og e.g. sensor data to "the cloud"
  
* [H4P_AsyncHTTP](h4phttp.md) Asynchronous remite acces of any http resource e.g. REST API to return any data of users' choice

* [H4P_UPNPServer](docs/upnp.md): provides full UPNP/SSDP device with Alexa voice control, PC Desktop OS integration

* [H4P_RemoteUpdate](docs/h4ru.md): OTA update from remote server

* [H4P_Heartbeat](docs/beat.md): Adds an "Up Time" to the web UI* 
  
* [H4P_LinkMaster](docs/link.md): Adds "slaving" of other H4 devices to create simple groups / scenarios. Swicth on/off the master and all slaves also switch on/off.


(* see :building_construction: [WiFi AP mode](ap.md) re configuration in AP mode )

### Network time, calendar scheduling

* [H4P_Timekeeper](docs/h4tk.md): NTP server sync for "clock time" alarms and scheduling

* [H4P_Sunrise](docs/h4tk.md#sunrisesunset) **EXPERIMENTAL** : Gets sunrise / sunset times from public API given Lat/Long

### Network presence detection

* :building_construction: [H4P_GateKeeper](gk.md) Manages creation of "enter" and "leave" events whern specifc roaming devices move in and out of netwrork range. Currently detectable device types are:

|Roeaming Device Class|Identified by|Example
| ---: | ---: | ---: |
|h4pRoamingDotLocal|MDNS protocol / service attributes|tcp,http|
|h4pRoamingH4|H4 device name|salon|
|h4pRoamingIP|IP Address|192.168.1.31|
|h4pRoamingUPNP|UPNP/SSDP property/value|USN,asds|

---
  
# Diagnostic / Logging / Development tools:

* [H4P_HeapWarn](docs/h4hw.md): Call user function on low Heap

* [H4P_QueueWarn](docs/h4qw.md): Call user function on low Queue

* [H4P_TaskSniffer](docs/h4ts.md): Low-level task / queue dumper for H4 + Plugins

## Loggers

* [H4P_EventListener](docs/ears.md): General-purpose listener calls user-defined function on any selection of events
  
* [H4P_SerialLogger](docs/slog.md): Event logging to serial monitor

* [H4P_LocalLogger](docs/llog.md): Event logging to LittleFS file

* [H4P_MQTTLogger](docs/mlog.md): Event logging to MQTT Server

* [H4P_RemoteLogger](docs/rlog.md): log to remote webserver

## Event Emitters

* [H4P_EmitHeap](docs/heap.md): Periodically Send `getFreeHeap()` value to event queue to help hunt memory leaks
  
* [H4P_EmitLoopCount](docs/loops.md): Send loops/sec to the event queue for diagnosing bottlencks (requires changes to [H4 config_plugins.h](https://github.com/philbowles/H4/blob/src/config_plugins.h) / compile options)
  
* [H4P_EmitQ](docs/eq.md): Periodically Send H4 Q size to event queue to help profling / performance tuning


* [H4P_EmitTick](docs/tick.md): Send synchronous 1x per second on-the-second heartbeat event.

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)