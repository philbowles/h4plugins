![H4P Logo](/assets/DiagLogo.jpg)

# Known Issues / Frequently-asked Questions

---

# Known issues

## webUI

There are a number of issues with the web user interface. They are due to 2 factors:

1. Reliance on AsyncWebserver library, even *after* its most egregious faults have been fixed. Unfortunately it is the best we have, so unless and until someone rewrites it or comes up with another from scratch, we are stuck with it. I already did that for Asynchronous MQTT to produce [PangolinMQTT](http://github.com/philbowles/PangolinMQTT) - there is not enough time in my life to go through something like *that* again

2. On an ESP8266 having a dynamic real-time updating web UI is pushing the limits of physics

As a result there are a number of factors to take into account:

* The UI can take a long time to load. It does this deliberately to avoid overloading AsyncWebserver and causing a crash. If the user has added many extra user fields, this can take a few seconds: be patient
* If you leave the browser open during development, and you re-upload a different or updated sketch - the browser and sketch are then "out-of-step" and this can (and frequently does) lead to "random" crashes. Simple answer: shut the browser down, and reopen on the newly loaded sketch. And ***DON'T raise an issue re "random" UI crashes during development!***
* Try to minimise the number of dynamically updating fields you add.
* If you ignore the previous advice, try not to leave the browser open for long periods of time. All of these things seem to aggravate AsyncWebserver code.
  
## HttpMySQLlogger

The supporting database has not yer been updated to reflect the new 1.0.1 event structure, so the example code will fail, and the documentation is out-of-date. Fixing both of those is in the "todo" list, but not particularly near the top.

## Presence Detection

Not particularly well tested, and H4 detection has been temporarily withdrawn. The whole susbsytem is to be rewritten soon(ish) (for 1.0.2?) to make much better use of the new event system. Therefore, any issues on this front will be well down the list of priorities. Be patient. I'm sure nobody even uses it so I don't even know why I'm writing this right now.

---

# Frequently-asked questions

## Why are you such a grumpy old b*gger?

An easy one to open with :). Because I do this for free in my spare time and it is immensely complex and time-consuming. It was designed for *me* , by *me* to make *my* home automation system work. If it somehow does not fit *your* requirements and *your* system, well basically that's tough. If on hearing that you are not happy, then basically it's even tougher, but it changes nothing. If you don't like it, don't use it, it's as simple as that. It makes no difference to me either way.

I share this project in the hope that it might help people and if it does that they will be grateful and happy. I do not get paid to smile politely in the face of stupidity and/or put up with rudeness from people who reap the benefits of my efforts and don't even say "thank you". Or who get heated because I refuse to change my design to suit *their* bizarre one-in-a-million edge case. Or who argue when I explain that it's their code at fault, not the library. Or who "won't be told". Or who say "my code doesn't work" without showing it to me or telling me how it breaks. Or who think because I put this up in public for my own entertainment that somehow I am now obliged to them or that somehow I owe them a duty to listen to them or care what they think or that there is some pressing need to fix *their* bug faster than I decide to, if at all...See also my reasons for removing support for AP mode. :smile:

I do this for fun. Help me keep it that way by obeying the one simple rule in this repo: ***Be polite or you will be blocked***.

## How do I get the device into AP mode?

You don't. Why would you even want to? This is a development framework for home deployments: You know what your router's SSID / Password are, hard-code them into your app. How often do you change them...hardly ever? Exactly. There *are* ways to change the SSID / password "in-flight" either via serial cable, HTTP/REST, MQTT etc. Obviously, if using MQTT, the app needs to know the local MQTT broker IP/port first, but... same as above. How often does that change, if ever? Hardcode it into the sketches you develop. It's what the system was designed for, and how it is designed to work. You have full access to the source of your app - you don't need AP mode!

## Why do I get compile errors if I set H4P_USE_WIFI_AP to 1? Is there a file missing?

Yes. It's called `H4P_WiFiAP.cpp` and it's missing for a good reason: AP mode is not supported in the this open source version of the library

## Why is AP mode not supported?

There are several reasons:

1. People failing to properly understand how it worked in earlier versions caused a lot of wasted support time.

2. 90% of the time people don't *actually* need it: they just (wrongly) *think* they do because "everything else has it" and/or they are being lazy and just want a quick and dirty way to change their devices during testing. If you change router, don't be lazy: edit your code and put the new credentials into it. If you have lots of devices, take advantage of OTA or even the automatic mass-update possible with [H4P_RemoteUpdate](rupd.md)

3. The 10% of people who *do* actually need it usually do so for one reason only: because they want to deploy their code into someone else's network, whose details they don't always know in advance, and they want that third party to be able to enter the credentials themselves...in the same way that most commercially-available devices do, but H4Plugins was never designed to work that way or be part of a commerical product.

H4Plugins was designed from the very beginning for home use. But those 10% of users who cuase 90% of the support issues are usually charging people money for that deployment and - presumably - support...which they then expect, nay, *demand* that *I* provide for free!

In summary then, it is to reduce my support burden by preventing the library from being used beyond its design parameters.

## How do I get a copy of the missing file H4P_WiFiAP.cpp ?

There are two routes:

1. By being a nice friendly, helpful beta tester, or a subscriber to my Youtube channel (once it gets going) or a [Patreon supporter](https://patreon.com/esparto) who agrees that it will be used purely for personal use and not deployed commercially for gain.

2. By contacting me to discuss a formal support contract. It's only going to be "a cup of coffee" type thing...I'm not doing this to make money, I'm doing it to prevent the abuse of my good nature by unscrupulous and/or rude people. If *you* are making money out of *my* work, I feel I should be getting at least *something* in return - Politeness and trust are always a good start.

For background: This is not a decision I have taken lightly. Technically, it took a *huge* amount of redesign and testing to de-couple AP mode from the rest of the project into one single file that a) breaks nothing when absent and b) "just works" when present.

Morally, it was *not* done for some fanciful fear of being taken avantage of, but from *actual experience* of getting "grief" and yes, even rudeness, from people deploying earlier versions for commercial gain. The equation did not balance for me: You use *my* work, *my* efforts to make money, you offer me nothing in return - not even thanks - yet *still* you feel you have the right to hound *me* for *free* support? It doesn't feel right somehow, does it?

I found it hard to devote my own free time to people raising issues including phrases such as "one of my clients..." or "my users don't like having to..." or "yes, but I have my customers to think of...". I always wanted to reply "Then use a commercial product and pay the going rate for modifications and support".

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)