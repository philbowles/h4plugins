![H4P Logo](/assets/GPIOLogo.jpg)

# Flasher Controller (short name="wink")

## Adds multiple LED blinking methods to H4 Universal Scheduler/Timer.

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---

# What does it do?

**H4Flasher** /*
 NOTATION
   Each "note" consists of 5 characters. They must all be present if a tune is not an exat multiple, it will not play.
   
   The first two characters are the value of the note. They ar either xN or x# for "natural" or "sharp" - there are no "flats"
   Thus for example Gb must be encoded as F# and so on. The note name MUST be uppercase.

   A rest is denoted by "R  "

   The third character is the octave. The fuill range of notes is from GN2 to BN7

   The fourth character is the time value:
   d  - demisemiquaver = 1/32 note
   s  - semiquaver = 1/16 note
   q  - quaver = 1/8 note
   c  - crothchet = 1/4 note
   m  - minim = 1/2 note
   b  - semibreve - full note

   The uppercase value of each is a "dotted" note, time*1.5 for example M is a dotted minim give a 3/4 note

   When a metrornome value is set in beats per minute, the crotchet(1/4) note is the yardstick and all other timings are
   scaled accordingly

   The last character is the "effect", either '-' meanning "slur": do not turn off note end but continue into the start of the next
   or: a number from 1 to 8 representing the volume of the note where 1 = lowest and 8 = full volume. When no effect is given, 
   the not is played at full volume: A space is the same as 8 

   For your convenience, vbars | can be inserted into the stream to help line everything up when composing
   the same is true for completely empty periods "     " (5 spaces). These are purely for alignment, have no musical significance
   and are stripped from the tune before playing
    
```cpp
/*
//
// Gymnopedie No. 1 - Erik Satie
//                 |                 |                 |                 |                 |                 |                 |
string gym_t1="R  m |F#6c5|R  m |EN6c5|R  m |F#6c5|R  m |EN6c5|R  c |F#7c |AN7c |GN7c |F#7c |C#7c |BN5c |C#7c |DN6c |AN6m |     |EN7c |F#6m |     |DN6c5|F#6m |     |EN7c5|F#6m |     |DN6c5|F#6m |     |EN7c5";
string gym_b1="R  m |DN6c5|R  m |C#6c5|R  m |DN6c5|R  m |C#6c5|R  c |R  c |DN6c |R  c |R  c |EN6c |R  c |R  c |AN6c |R  c |R  c |C#6c |R  c |R  c |AN6c5|R  c |R  c |C#6c5|R  c |R  c |AN6c5|R  c |R  c |C#6c5"; 
string gym_c1="GN3m5|BN5c5|DN3m |AN5c5|GN3m5|BN5c5|DN3m5|AN5c5|GN3m |     |BN5c |DN3m |     |AN5c |GN3m |     |BN5c |DN3m |     |AN5c |GN3m5|     |BN5c5|DN3m5|     |AN5c5|GN3m5|     |BN5c5|DN3m5|     |AN5c5"; 
string gym_t2="R  c |F#7c |AN7c |GN7c |F#7c |C#7c |BN5c |C#7c |DN6c |AN6m |     |EN7c |F#6m |     |DN6c |F#6m6|     |EN7c6|F#6m4|     |DN6c4|F#6m2|     |EN7c1";
string gym_b2="R  c |R  c |DN6c |R  c |R  c |EN6c |R  c |R  c |AN6c |R  c |R  c |C#6c |R  c |R  c |AN6c |R  c |R  c |C#6c6|R  c |R  c |AN6c4|R  c |R  c |C#6c1"; 
string gym_c2="GN3m |     |BN5c |DN3m |     |AN5c |GN3m |     |BN5c |DN3m |     |AN5c |GN3m |     |BN5c |DN3m6|     |AN5c6|GN3m4|     |BN5c4|DN3m3|     |AN5c1"; 
// assemble parts into single strings
string gym_t=gym_t1+gym_t2;
string gym_b=gym_b1+gym_b2;
string gym_c=gym_c1+gym_c2;

H4P_TUNE  gymnopedie1 ={
  {vox_t,gym_t}, // use voice vox_t to play right-hand melody part (treble clef) on D8
  {vox_b,gym_b}, //       "      _b          left hand part (bass clef) on D7 
  {vox_c,gym_b}  //       "      _c         extra channel for 3-note chords  
};
 */
```

---

# Usage

```cpp
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop
H4P_FlasherController h4fc;
```

## Dependencies

none, but must be created *after* [**H4P_SerialCmd**](h4cmd.md) if using that plugin. Also if [**H4P_GPIOManager**](h4gm.md) is in use, the relevant pin will be automatically set as output - otherwise it is up to the user to call `pinMode` before any API function below and to manage its logical / physical and active high /active low states. The simplest solution is to always also include [**H4P_GPIOManager**](h4gm.md) before H4P_FlasherController.

## Commands Added

none *TODO* add dynamic command-line control

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

[Basic Example sketch](../examples/H4FC_Basic/H4FC_Basic.ino)

[Active Low Example sketch](../examples/H4FC_ActiveLow/H4FC_ActiveLow.ino)

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
