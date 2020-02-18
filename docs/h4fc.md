![H4P Logo](/assets/GPIOLogo.jpg)

# Flasher Controller (short name="wink")

## Adds multiple LED blinking methods to H4 Universal Scheduler/Timer.

![H4Flasher](/assets/H4flasher.jpg)

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---

# What does it do?

**H4Flasher** Manages multiple timers provided by **H4** to control as many LEDs as you require at the same time, each requiring only one line of code.
LEDs flash independently at different rates using one of four methods:

* Simple symmetric Square wave: (on for T, off for T)
* PWM: on for n% of time period T, off for 100-n%
* Pattern: Arbitrary on-off sequence defined by e.g. "10000110001" with user-defined timebase
* Morse: e.g. "...---..." for "SOS". Compile-time option for text-to Morse translation

---

# Usage

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS
H4P_FlasherController h4fc;
```

## Dependencies

none, but must be created *after* [**H4P_SerialCmd**](h4sc.md) if using that plugin. Also if [**H4P_GPIOManager**](h4gm.md) is in use, the relevant pin will be automatically set as output - otherwise it is up to the user to call `pinMode` before any API function below and to manage its logical / physical and active high /active low states. The simplest solution is to always also include [**H4P_GPIOManager**](h4gm.md) before H4P_FlasherController.

## Commands Added

none *TODO* add dynamic command-line control

## Unloadable

NO:

---

# A word about active-HIGH and active-LOW

(Experts should skip to the next section). Most peolple think of LEDs in terms of "ON" and "OFF" rather that digtal states HIGH and LOW. To compilcate things, LEDs can be physically wired two ways: "active HIGH" which follows the "normal" way of thinking about digital GPIOs: set the pin HIGH and the LED turns ON.

Unfortunately many boards (e.g. ESP8266 and ESP32 have their onboard LEDs wired as "active LOW" where setting the GPIO to LOW turns the LED ON, the exact opposite of active HIGH and confusing to many people. When the user wants to stop an LED flashing, it needs to be turned OFF whether it is active-HIGH or active-LOW. In order to do this correctly, **H4Flasher** needs to know if the LED is wired active-HIGH or active-LOW. The final parameter to all of the API calls that create a flashing pin is "active" and defaults to HIGH.
  
(* *The output device does not have to be an LED - it is just that this is the most common and easiest to understand. It can be any GPIO-driven device that does react badly to rapid switching*)

---

# API

All times in milliseconds

```cpp
// Simple flash, 50% square wave: ON for rate ms, OFF for rate ms
flashLED(uint32_t rate, uint8_t pin = LED_BUILTIN,uint8_t active=HIGH);

// Takes pattern of dots dashes and spaces: "... --- ..." ("SOS") and a timebase which controls repetition speed
// 200ms is a good starting point for the right "feel"
flashMorse(const char *pattern, uint32_t timebase, uint8_t pin = LED_BUILTIN,uint8_t active=HIGH);

// NB only available if you edit the H4PConfig.h file and uncomment the define for H4F_MORSE_SUPPORT
// Translates text (no numerals or punctuation, [A-z a-z] only)
flashMorseText(const char * pattern,uint32_t timebase,uint8_t pin=LED_BUILTIN,uint8_t active=HIGH);

// Arbitray pattern of 1 and zero, = ON / OFF, irrespective of active-HIGH/LOW using timebase to set rate
// e.g. flashPattern("1000001001",250,MY_LED); // default of active-HIGH assumed
flashPattern(const char * pattern,uint32_t timebase,uint8_t pin=LED_BUILTIN,uint8_t active=HIGH);

// PWM-style flash of duty% of period. 1000,25 gives ON 250ms OFF 750ms, ON 250ms etc
flashPWM(uint32_t period,uint8_t duty,uint8_t pin=LED_BUILTIN,uint8_t active=HIGH);

// Utility function which returns true if pin has a flasher attached (NOT whether its actually glowing!)
isFlashing(uint8_t pin=LED_BUILTIN);

// Single short blip of period ms: best when kept very short e.g. sub 100ms. Great for debugging!
pulseLED(uint32_t period,uint8_t pin=LED_BUILTIN,uint8_t active=HIGH);

// Have a guess! Sets the LED to the opposite of its active state and cleans up any timers, patterns etc
stopLED(uint8_t pin=LED_BUILTIN);
```

[Basic Example sketch](../examples/H4FC_Basic/H4FC_Basic.ino)

[Active Low Example sketch](../examples/H4FC_ActiveLow/H4FC_ActiveLow.ino)

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
