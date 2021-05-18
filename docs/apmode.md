
![H4P Flyer](../assets/WiFiLogo.jpg) 

# AP Mode

# Design Philosophy 1

H4Plugins was designed from the outset to be deployed inside a small-perimeter network that did not require remote access and will still function if none exists. Also let's remind ourselves that it's an open-source *programming framework*, not a proprietary drop-in binary firmware. The source is open, visible and indeed it is an *essential requirement* to edit the source to get H4Plugins running. It is designed to operate as a station in infrastructure mode ("`WIFI_STA`") so the first question we need to ask ourselves is: "Why would you ever need it to run in AP mode?"

The answer is: "You never do".

The fact that you might *want* to use AP mode is:

* entirely up to you
* easy enough to implement yourself if you are a competent programmer
* free to do in an open-source scenario
* fundamentally *wrong* in terms of H4Plugins philosophy and outside of its design goals.

The recommended way to make H4Plugins connect to a new SSID is:

*EDIT THE CODE OF YOUR SKETCH TO INCLUDE YOUR SSID / PASSWORD*

Because most folk only change those things once every few years if at all, it makes it the most remote and darkest of "corner cases". When we factor in that ESP8266 is tiny, this firmware is large and complex and it already has to be cut-down and shoe-horned into place in many other ways, it makes no sense to include code that will probably only ever get used *once* in the lifetime of the device.

It is immediately self-evident a that one-time-only AP mode configuration code not only "fits the bill" of things to cut out, but is practically the definition of it.

# :cherry_blossom: (Optional) Design Philosophy 2

## When devices *usually* use AP mode

When you buy a commercial device, it has no way of knowing your SSID and so there is usually some hardware-induced "pairing mode" whereby you then connect to it as an AP with e.g. a mobile phone, tell it your network and then *never need to do it again*.

The key word here is "commercial". H4Plugins was never designed to be used as a mass roll-out con$umer "product". It's open-source: if you want to use it that way, then you can fork it, hack it and do what you want with it, including charging people money to install it on their systems - I can't stop you doing that. But I don't have time to offer free support to people who take that track.

## Easier / Better ways to do it...

H4Plugins provides other methods (*far* better if you have many devices) to get a new SSID into your device(s). For example, using "generic" sketches, by which I mean code that does not have any device-specific details hard-coded into it. This can be uploaded to *any* device where it will then - somehow - configure itself, much like the commercial example discussed above.

The devil is in the detail of the "somehow" in that last sentence - there are many different ways. The way that H4Plugins does it (*without* AP mode, of course) is discussed in detail in :building_construction: [Advanced Topics](docs/advanced.md) under "Generic Deployment".

My first response when folk ask "how do I connect to a new SSID?" is to repeat: 

EDIT THE CODE OF YOUR SKETCH TO INCLUDE THE NEW SSID / PASSWORD

...job done.

"But I have dozens of devices!" they complain...

"No problem" I reply, "Do the following"

* Read :building_construction: [Advanced Topics](docs/advanced.md) under "Generic Deployment"
* Make sure you code your sketches the recommend way described in that document
* Include :building_construction: [H4P_RemoteUpdate](h4pxxxx.md)
* Recompile one sketch with the new SSID, increase its version number and deploy to the remote update server
* In any MQTT client, publish `all/rupd/fw` and watch as all your devices magically update themselves

Note that "AP Mode" is not required and never mentioned. Trying to do anything similar with AP mode would be utterly impractical and that's the final nail in its coffin. With H4Plugins there is usually a much better ways of doing whatever it you (wrongly :) ) think you need AP Mode for - just ask!

---

# OK, I hear all that but *I still want AP Mode* (so I can deploy it to my customers)

I repeat "... I don't have time to offer free support to people who take that track...", but I'm happy to discuss a support contract or paid collaboration in deploying your H4-based code in a commercial setting.

Ready, tested and waiting is: `H4P_WiFiAP.cpp` which allows you to deploy totally generic binaries* that will start up by default in AP Mode and can be forced back into AP mode via a variety of methods

* long press on GPIO-linked physical button
* total wipe of SPIFFS / FS, most commonly by:
* command `h4/factory` via Serial, MQTT, HTTP REST etc

It provids also provides a modified user-input version of the webUI whcih allows typical "enter your own details" scenarios for:

* SSID / password
* MQTT broker
* Remote update server

Also, be reminded that H4Plugins works as described *without* that file and is fully open-source in that regard. If you choose to implement the optional AP Mode feature, you need to pre-arrange some form of agreement with me. 

*As H4Plugins is provided under an open source licence, you are required to make your derived sources publicly available too, including - of course - to your customers.

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)