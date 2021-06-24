![H4P Logo](../assets/H4PLogo.png)

# Documentation Master Index

:heavy_check_mark: = As good as it's likely to get for this release

:building_construction: = Missing / Work in Progress

:gem: = Absolutely essential: without reading this you will find it hard going.

:cherry_blossom: = Interesting but not 100% essential: a "sidebar article"

:door: = "external" - takes the reader away from the current page, perhaps even offsite

---

* :cherry_blossom: [Why the name "H4"? A little history](h4.md) :heavy_check_mark:
* :gem: :door: [H4 and event-driven programming](https://github.com/philbowles/H4#why-do-i-need-it)  :heavy_check_mark:
* :gem:[Everything is an event: Listeners, Emitters and Handlers](events.md) :heavy_check_mark:
* :gem:[Global Variables and persistent storage](globals.md) :heavy_check_mark:
* :gem:[Common Command and Control: H4P_SerialCmd and Services](ccc.md) :heavy_check_mark:
* :cherry_blossom: [Static and Utility functions](statics.md) :heavy_check_mark:
* [GPIO Handling and "NODE-PINK" Introduction](nodepink.md) :heavy_check_mark:
  * [GPIO/NODE-PINK: a logical approach](logphys.md) :heavy_check_mark:
  * [GPIO/NODE-PINK: basic flows](basic.md) :heavy_check_mark:
  * [GPIO/NODE-PINK: rotary encoder flows](encoders.md) :heavy_check_mark:
  * :building_construction: [GPIO/NODE-PINK: analog flows](analog.md)
  * :building_construction: [GPIO/NODE-PINK: advanced techniques](nodepinkadv.md)
* ["Things" and "Switches"](things.md) :heavy_check_mark:
* [WiFi and the web UI](webui.md) :heavy_check_mark:
* [WiFi AP mode](apmode.md) :heavy_check_mark:
* [Handling MQTT topics / commands](mqcmds.md) :heavy_check_mark:
* [Network Presence Detection](presence.md) :heavy_check_mark:
* Advanced Topics
    * [Generic Binaries](generix.md) :heavy_check_mark:
    * [Running a remote update server](rusrv.md) :heavy_check_mark:
    * :building_construction: [Additional board defintions](boards.md)
    * :building_construction: writing your own plugin
* [Known Issues / FAQs](docs/faq.md) :heavy_check_mark:
* [Raising an Issue](docs/issues.md) :heavy_check_mark:
* Plugins APIs
  * [H4P_AsyncHTTP](h4phttp.md) :heavy_check_mark:
  * [H4P_AsyncMQTT](h4mqtt.md) :heavy_check_mark:
  * [H4P_BinarySwitch](swings.md) :heavy_check_mark:
  * [H4P_BinaryThing](swings.md) :heavy_check_mark:
  * [H4P_ConditionalSwitch](swings.md) :heavy_check_mark:
  * [H4P_ConditionalThing](swings.md) :heavy_check_mark:
  * [H4P_EmitHeap](heap.md) :heavy_check_mark:
  * [H4P_EmitLoopCount](loops.md) :heavy_check_mark:
  * [H4P_EmitQ](eq.md) :heavy_check_mark:
  * [H4P_EmitTick](tick.md) :heavy_check_mark:
  * [H4P_EventListener](ears.md) :heavy_check_mark:
  * [H4P_GateKeeper](gk.md) :heavy_check_mark:
  * [H4P_HeapWarn](h4hw.md) :heavy_check_mark:
  * [H4P_Heartbeat](beat.md) :heavy_check_mark:
  * [H4P_LinkMaster](h4plink.md) :heavy_check_mark:
  * [H4P_LocalLogger](llog.md) :heavy_check_mark:
  * [H4P_MQTTLogger](mlog.md) :heavy_check_mark:
  * [H4P_PinMachine](h4pm.md):heavy_check_mark:
  * [H4P_QueueWarn](h4qw.md) :heavy_check_mark:
  * [H4P_RemoteLogger](rlog.md) :heavy_check_mark:
    * [MySQL event logger webserver](https://github.com/philbowles/mysqlrest) :heavy_check_mark:
  * [H4P_RemoteUpdate](rupd.md)
  * [H4P_SerialCmd](h4p.md) :heavy_check_mark:
  * [H4P_SerialLogger](ears.md) :heavy_check_mark:
  * [H4P_Signaller](h4fc.md)
  * :building_construction: [H4P_Sunrise](h4pxxxx.md)
  * :building_construction: [H4P_TaskSniffer](h4pxxxx.md)
  * :building_construction: [H4P_Timekeeper](h4pxxxx.md)
  * :building_construction: [H4P_ToneController](h4pxxxx.md)
  * [H4P_UPNPServer](upnp.md) :heavy_check_mark:
  * [H4P_WiFi](h4wifi.md) :heavy_check_mark:
  * :building_construction: [H4P_Voice](h4pvox.md)
  
 ---

 # [H4Plugins Youtube channel](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)

 ## "Arduino Turquoise" label tutorials

 * [0: Version 3 taster](https://www.youtube.com/watch?v=4ySOh0ukyrc)
 * [1: Blinky on steroids](https://www.youtube.com/watch?v=SRHze-LRvN4)
 * [2: Why you need it](https://www.youtube.com/watch?v=OvOz3QPGoY0)
 * [3: GPIO simple start RAW input](https://www.youtube.com/watch?v=k-TgHK5qTWc)
 * [4: GPIO Filtering & Debouncing](https://www.youtube.com/watch?v=GflSWkZcr9g)
 * [5: GPIO NODE-PINK and Debouncing descendants](https://www.youtube.com/watch?v=VUAuQOKkLLY)
 * [6: GPIO The other children of debouncing: it's about time](https://www.youtube.com/watch?v=n6Y5OM-w2Kc)
 * [7: GPIO Stragglers and DIY plumbing](https://www.youtube.com/watch?v=SSMLsgJKotA)
 * [8: GPIO Rotary encoders need not be an Enigma](https://www.youtube.com/watch?v=4ySOh0ukyrc)
 * [9: GPIO Analog Inputs](https://www.youtube.com/watch?v=1oxsNVHloqA)
 * [10: Advanced Plumbing](https://youtu.be/yyyAttshSV0)
 * [11: Internet of Things ain't what they used to be](https://youtu.be/fLUKDAboAU8)
 * [12: Internet of Things can only get better](https://www.youtube.com/watch?v=sy7xHuKghrM)
 * [13: Target in Range!: Network Presence Detection](https://www.youtube.com/watch?v=tLeptP3D5fw)
 * [14: Running the installer](https://youtu.be/Q1givForuEQ) :new:

## Red label "Expert" series

* [1. Running a remote update server on Nide-RED](https://youtu.be/zNjafa-4QEw)
* [2. Slaying C++ myths](https://youtu.be/0BFs84V5Kxg)

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)