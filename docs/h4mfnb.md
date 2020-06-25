![H4P Flyer](/assets/GPIOLogo.jpg) 

## Three Function Button (short name="mfnb")

### Adds 3-function* GPIO button to H4 Universal Scheduler/Timer

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---
## What does it do?

Allows the user to control a [H4P_BinarySwitch](things.md) or a[H4P_BinaryThing](things.md) by pressing and holding a simple non-latching 'tact' button. The button causes 3 different actions to occur depending on how long it held down.

In essence it links an output GPIO (defined by the xSwitch or xSource plugin) to an input GPIO connector and uses an LED to signal its changing state the longer the button is held.

A "short" press (less than 2 seconds**) simply switches the device to the opposite state as a "normal" press of any button might do. The resulting action depends on the definition of the linked switch plugin.

A "medium" press over 2 seconds (but less than 5 seconds**) starts the associated LED flashing rapidly and when the button is released, the device will reboot.

A "long" press - anything over 5 seconds** - starts the LED flashing extremely rapidly and when the button is released, the device will clear any stored configuration information and then reboot, a process known as a "factory reset".

**N.B** If the [H4P_WiFi](h4wifi.md) plugin is in use, the factory reset will clear any stored WiFi credentials, [H4P_PersistentStorage](h4stor.md) items etc

(* ***Four** functions if you count the LED flashing* :) )

(** These default values can be changes by the user - see "Tweakables" below)

---

# Usage

H4P_MultiFunctionButton is a specialised example of an IOT "Source" - you should read ["Sources vs Switches"](things.md) before continuing

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_GPIOManager h4gm;
// either
//H4P_BinarySwitch h4onof(...
or
//H4P_BinaryThing h4onof(...
H4P_MultiFunctionButton h4mfb(...
```

# Dependencies

Requires a GPIO pin to be connected to an LED (default is LED_BUILTIN) and a "tact" style button on a 2nd GPIO

# Commands Added

none

---

# API

```cpp
/* constructor: */
H4P_MultiFunctionButton(
//  the input button
    uint8_t pin,
    uint8_t mode,
    H4GM_SENSE b_sense,
    uint32_t dbTimeMs, // debounce time
//          the linked LED for message flashing
    uint8_t led,
    H4GM_SENSE l_sense);
);
```

[Example code](../examples/XTRAS/H4P_SONOFF_Basic/H4P_SONOFF_Basic.ino)

---

# Tweakables

The following values are defined in `config.h` . They are chosen initally to set a good balance between stability, performance and memory / stack usage. *It is not advisable to change them unless you know exactly what you are doing and why*. 

**N.B.** **Support will not be provided if any of these values are changed.**

* H43F_MEDIUM            175
Millisecond flash rate of a medium press

* H43F_FAST               50
Millisecond flash rate of a long press

* H43F_TIMEBASE          175
Timebase of the Morse S-O-S flash when signalling onWiFIDisconnect (if [H4P_WiFi](h4wifi.md) is used)
* H43F_REBOOT           2000
Millisecond time defining transition from short->medium
* H43F_FACTORY          5000
* Millisecond time defining transition from medium->long

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
