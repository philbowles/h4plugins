![H4P Flyer](/assets/GPIOLogo.jpg) 

# Basic Switch

## Adds elementary GPIO on/off/toggle functions to H4 Universal Scheduler/Timer.

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---

# What does it do?

H4P_BasicSwitch wraps a GPIO pin in an object that allows control by simple commands that are then available to [H4P_SerialCmd](h4scmd.md), [H4P_AsyncWebServer](h4asws.md)'s HTTP REST API, or [H4P_MQTT](h4mqtt.md) without requring them to know the GPIO pin number.

---
# Usage

```cpp
#include <H4Plugins.h>
H4P_GPIOManager h4gm;
H4P_BasicSwitch h4onof;
```

## Dependencies

* [H4P_GPIOManager](h4gm.md) Plugin

**N.B.** *H4P_BasicSwitch represents a **default** action: it cannot be used simultaneously with any other plugin that also defines a default action, e.g. [H4P_UPNPSwitch](h4upnp.md).*

## Commands Added

* h4/off
* h4/on
* h4/switch/n (payload = 1 or 0)
* h4/toggle (invert current state)

## Topics automatically published

If [H4P_MQTT](h4mqtt.md) is also used, this plugin publishes `h4/< your device name >/state` with a payload set to the current state whenever the state changes

c## Trusted Name

*ONOF*

## Unloadable

No

---

# API

```cpp
/* Constructor
pin is the GPIO output which gets "switched" when the state changes
sense is ACTIVE_HIGH or ACTIVE_LOW depending on the device
inital is the starting state ON or OFF
onChange is the name of  a user function that gets called after the state change with b set to the current state see GPIOManager plugins for more details

*/
H4P_BasicSwitch(uint8_t pin,H4GM_SENSE sense, uint8_t initial,H4BS_FN_SWITCH onChange=[](bool){});

void turnOff();
void turnOn();
void toggle(); // invert state
void turn(bool b); // b = new state: 1 or 0; true/false; ON/OFF

```

[Example Sketch - Basic](../examples/H4P_BasicSwitch/H4P_BasicSwitch.ino)
[Example Sketch - with 3-function button](../examples/H4P_BasicSwitch3fnb/H4P_BasicSwitch3fnb.ino)
[Example Sketch - with MQTT](../examples/H4P_BasicSwitchMQTT/H4P_BasicSwitchMQTT.ino)

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
