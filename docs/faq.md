![H4P Logo](../assets/DiagLogo.jpg)

# Known Issues / Frequently-asked Questions

---

# Known issues

## webUI

There are a number of issues with the web user interface. They are due to 2 factors:

1. Reliance on AsyncWebserver library, even *after* its most egregious faults have been fixed. Unfortunately it is the best we have, so unless and until someone rewrites it or comes up with another from scratch, we are stuck with it. I already did that for Asynchronous MQTT (which was 10x worse!) to produce [PangolinMQTT](http://github.com/philbowles/PangolinMQTT) - there is not enough time in my life to go through something like *that* again

2. On a tiny ESP8266, simply having a dynamic real-time updating webUI is pushing the limits of physics.

As a result there are a number of factors to take into account:

* The UI can take a little time to load. It does this deliberately to avoid overloading AsyncWebserver and causing a crash. If the user has added many extra user fields, be patient - hitting refresh will just double the workload, making a crash even *more* likely
  
* If you leave the browser open during development, and you re-upload a different or updated sketch - the browser and sketch are then "out-of-step" and this can (and frequently does) lead to "random" crashes. Simple answer: shut the browser down, and reopen on the newly loaded sketch. And ***DON'T raise an issue re "random" UI crashes during development!***
  
* Try to minimise the number of dynamically updating fields you add.
  
* If you ignore the previous advice, try not to leave the browser open for long periods of time. All of these things seem to aggravate AsyncWebserver code.
  
Regular updates e.g. 1x per second "Up Time" are anything but smooth.. they will jump / stutter / lag etc as AsyncWebserver fills its queue and the webUI has to buffer (or ultimately abandon) its updates to avoid a crash.

## H4P_AsyncHTTP

Is known to be very pernickety about the type of JSON it will decode  correctly without crashing. It is a deliberately "cheap n cheezy" implemention so thaty your own internal websuites can send small, simple data cheaply and rapidly. For anything more serious, use a proper library like [ArduinoJson]( !REF!!! ) 

There are also some sites that behave oddly when chunking data, so incomplete / strange results may occur ion some sites. This is a "feature" of the underlying library [ArmadilloHTTP](https://github.com/philbowles/ArmadilloHTTP) and will be fixed in a later release.

That library also does not follow redirects - another item on ots "TODO" list.

---

# Frequently-asked questions

## How do I get the device into AP mode?

You don't. Why would you even want to? This is a development framework for home deployments: You know what your router's SSID / Password are, hard-code them into your app. How often do you change them...hardly ever? Exactly. Read [WiFi AP mode](apmode.md) H4Plugins does not need AP mode.

## Why do I get compile errors if I set H4P_USE_WIFI_AP to 1? Is there a file missing?

Yes. It's called `H4P_WiFiAP.cpp` and it's missing for a good reason: AP mode is not supported in the this open source version of the library Read [WiFi AP mode](apmode.md).

## Why is AP mode not supported?

There are several reasons, Read [WiFi AP mode](apmode.md) for the best ones.
## How do I get a copy of the missing file H4P_WiFiAP.cpp ?

By contacting me to discuss a formal support contract. 

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)