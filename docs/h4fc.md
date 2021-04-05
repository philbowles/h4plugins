![H4P Logo](/assets/GPIOLogo.jpg)

# H4P_Signaller 

## Shortname wink

Flash as many LEDs as you require at the same time with different rates and/or patterns, each requiring only one line of code.

* Simple symmetric Square wave: (on for T, off for T)
* PWM: on for n% of time period T, off for 100-n%
* Pattern: Arbitrary on-off sequence defined by e.g. "10000110001" with user-defined timebase
* Morse: e.g. "...---..." for "SOS". Compile-time option for text-to Morse translation

These are "fire-and-forget" functions: once started, the LED "does its thing" while your code continues handling other tasks.

## A quick word about "ACTIVE HIGH" and "ACTIVE LOW"

 Many people think of LEDs in terms of "ON" and "OFF" rather than digital states HIGH and LOW or 1 and 0. To complicate things, LEDs can be physically wired two ways: "active HIGH" which follows the "normal" way of thinking about digital GPIOs: set the pin HIGH and the LED turns ON.

Unfortunately many boards (e.g. ESP8266 and ESP32 have their onboard LEDs wired as "active LOW" where setting the GPIO to LOW turns the LED ON, the exact opposite of active HIGH and confusing to many people. When the user wants to stop an LED flashing, it needs to be turned OFF whether it is active-HIGH or active-LOW. In order to do this correctly, H4P_Signaller needs to know if the LED is wired active-HIGH or active-LOW. The final parameter to all of the API calls that create a flashing pin is "active" and defaults to HIGH.
  
(*The output device does not have to be an LED - it is just that this is the most common and easiest to understand. It can be any GPIO-driven device that does react badly to rapid switching*)

---

## Contents

* [Usage](#usage)
* [Dependencies](#dependencies)
* [Commands Added](#commands-added)
* [Events Emitted](#s-emitted)
* [Events Listened for](#s-listened-for)
* [Tasks Run](#tasks-run)
* [Service Commands](#service-commands)
* [API](#api)

---

# Usage

```cpp
H4P_Signaller h4fc;
```

This plugin is a "singleton" - there may be only one single instance of it in the app. 
It may be instantiated as any name the user chooses, prefix all API calls below with `myChosenName.`

# Dependencies

[**H4P_PinMachine**](h4gm.md)

### Commands Added

N/A

*TODO* add commands to e.g. create flashers @ runtime

# Events Listened for

N/A

# Events Emitted

N/A

# Tasks Run

| Symbolic Name | Short Name | Type | Singleton | Purpose |
| :----------   | :--- | :--- | :-------: | :---    |
|H4P_TRID_PP1x|PP1X|once||Pulse OFF timer|
|H4P_TRID_PWM1|PWM1|every||PWM period counter|
|H4P_TRID_PATN|PATN|every||Pattern sequencer/rotator|

# Service Commands

`stop` has no effect

`start` has no effect

## Callback functions

N/A

---

# API

All times in milliseconds

```cpp
// Constructor
H4P_Signaller();

// Simple flash, 50% square wave: ON for rate ms, OFF for rate ms
flashLED(uint32_t rate, uint8_t pin = LED_BUILTIN,uint8_t active=HIGH);

// Takes pattern of dots dashes and spaces: "... --- ..." ("SOS") and a timebase which controls repetition speed
// 200ms is a good starting point for the right "feel"
flashMorse(const char *pattern, uint32_t timebase, uint8_t pin = LED_BUILTIN,uint8_t active=HIGH);

// NB only available if you edit the config.h file and uncomment the define for H4F_MORSE_SUPPORT
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

## Example sketches

* [FlashLED](../examples/FLASHER/H4FC_flashLED/H4FC_flashLED.ino)
* [FlashMorse](../examples/FLASHER/H4FC_flashMorse/H4FC_flashMorse.ino)
* [FlashPattern](../examples/FLASHER/H4FC_flashPattern/H4FC_flashPattern.ino)
* [FlashPWM](../examples/FLASHER/H4FC_flashPWM/H4FC_flashPWM.ino)
* [Mayhem: multiple different simultaneous LEDs](../examples/FLASHER/H4FC_Mayhem/H4FC_Mayhem.ino)
* ["White Noise" rapid random pulsing](../examples/FLASHER/H4FC_WhiteNoise/H4FC_WhiteNoise.ino)

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)
