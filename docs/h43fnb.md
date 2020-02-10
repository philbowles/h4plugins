![H4P Flyer](/assets/GPIOLogo.jpg) 

## Three Function Button

### Adds 3-function* GPIO button to H4 Universal Scheduler/Timer

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---
## What does it do?

Allows the user to control a switch plugin, either a [H4P_BasicSwitch](h4onof.md) or a [H4P_UPNPSwitch](h4upnp.md) by pressing and holding a simple non-latching 'tact' button. The button causes 3 different actions to occur depending on how long it held down.

In essence it links an output GPIO (defined by the swicth plugin) to an input GPIO and uses an LED to signal its changing state.

A "short" press (less than 2 seconds) simply switches the device to the opposite state as a "normal" press of any button might do. The resulting action depends on the definition of the linked switch plugin.

A "medium" press over 2 seconds (but less than 5 seconds) starts the associated LED flashing rapidly and when the button is released, the device will reboot.

A "long" press - anything over 5 seconds - starts the LED flashing extremely rapidly and when the button is released, the device will clear any stored configuration information and then reboot, a process known as a "factory reset".

**N.B** If the [H4P_WiFi](h4wifi.md) plugin is in use, the factory reset will clear any stored WiFi credentials therefore ensuring the when the device reboots, it will start in AP configuration mode.

(* ***Four** functions if you count the LED flashing* :) )

---

# Usage

```cpp
#include <H4Plugins.h>

H4P_GPIOManager h4gm;
// either
H4P_UPNPSwitch h4upnp(...
//or
H4P_BasicSwitch h4upnp(...
H4P_ThreeFunctionButton h43fb(...
```

# Dependencies

Requires a GPIO pin to be connected to an LED (default is LED_BUILTIN) and a "tact" style button on a 2nd GPIO

# Commands Added

none

# Callbacks

```cpp
void onChange(uint32_t sweptValue); // called when swept value changes
```

# Trusted Name

*TFNB*

# Unloadable

NO

---

## API

```cpp
/* constructor: */
H4P_ThreeFunctionButton(
    H4P_BasicSwitch* bsp,  //a reference to a previoulsy defined H4P_BasicSwitch or H4P_UPNPSwitch
    uint32_t dbTimeMs=15, // the switch debounce value in milliseconds: depends on the individual switch
    //          the input button
    uint8_t pin=BUTTON_BUILTIN,
    uint8_t mode=INPUT, // or INPUT_PULLUP depending on the hardware
    H4GM_SENSE b_sense=ACTIVE_LOW, // the "sense" of the button
//          the linked LED for message flashing
    uint8_t led=LED_BUILTIN, // GPIO# of the linked LED
    H4GM_SENSE l_sense=ACTIVE_LOW // the active "sense" of the linked LED
    );
```

[Example code](../examples/H43FNB/H4P_SONOFF_Basic/H4P_SONOFF_Basic.ino)

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
