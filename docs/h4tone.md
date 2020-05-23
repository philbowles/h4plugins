![H4P Logo](/assets/GPIOLogo.jpg)

# Tone Controller (short name="tone")

## Adds buzzer tones and multiple voice "music" to H4 Universal Scheduler/Timer.

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---

# What does it do?

One-liner: It makes noises. The most important thing to understand is that you need a *passive* buzzer. If you accidentally try an *active* buzzer you will get *one* tone only and you will tell me the library doesn't work. **USE A PASSIVE BUZZER!**

H4P_ToneController has two modes: making simple beeps/sirens and playing what vaguely approaches "music" with separate "voices" on multiple buzzers. Using the "music" method you need o understand the notation first:

# Notation

   Each note consists of 5 characters. They must all be present: if a tune is not an exact multiple of 5, it simply will not play.

   The first two characters are the value of the note. They are either xN or x# for "natural" or "sharp" - there are no "flats"
   Thus for example Gb must be encoded as F# and so on. The note name MUST be uppercase.

   The third character is the octave. The full range of notes is from GN2 to BN7

   The fourth character is the time value:

   * d  - demisemiquaver = 1/32 note
   * s  - semiquaver = 1/16 note
   * q  - quaver = 1/8 note
   * c  - crothchet = 1/4 note
   * m  - minim = 1/2 note
   * b  - semibreve - full note

   The uppercase value of each is a "dotted" note = T*1.5. For example M is a dotted minim give a 3/4 note

   A rest is denoted by "R  "

   When a metronome value is set in beats per minute, the crotchet(1/4) note is the yardstick and all other timings are
   scaled accordingly.

   The last character is the "effect", either '-' meanning "slur": do not turn off note end but continue into the start of the next
   or: a number from 1 to 8 representing the volume of the note where 1 = lowest and 8 = full volume. When no effect is given, 
   the not is played at full volume: A space is the same as 8 

   For your convenience, vbars | can be inserted into the stream to help line everything up when composing
   the same is true for completely empty periods "     " (5 spaces). These are purely for alignment, have no musical significance
   and are stripped from the tune before playing.

   ## Notation Example
    
```cpp
/*
//
// Gymnopedie No. 1 - Erik Satie
//
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
H4P_ToneController h4tc(...
```

## Dependencies

none

## Commands Added

none *TODO* add dynamic command-line control

## Data Structures

H4P_STAVE  pair<H4P_Voice&,const string&>;
H4P_TUNE   vector<H4P_STAVE>;

---

# API

All times in milliseconds

```cpp
// constructor
H4P_ToneController(uint32_t tempo=60); // default Beats per minute

uint32_t length(const string& tune); // milliseconds playing time of tune for syncing one tune after another
void metronome(uint32_t tempo); // change tempo: Beats per minute
void multiVox(H4P_TUNE tune,uint32_t tempo,int transpose=0); // play multiple "voices" simultaneously transposed by N semitones +/-
string setVolume(const string& s,uint8_t volume); // adjust volume of whole tune
void tone(uint8_t pin,uint32_t freq,uint32_t duration=0,uint8_t volume=8); // play single tone of frequency f on pin for < duration > milliseconds (0=continuous) at volume 1 (quietest) to 8 (loudest)
```

[Example Sketch](../examples/MUSIC/H4P_Simple_Tones/H4P_Simple_Tones.ino)

```cpp
//
// Siren macros
// p = pin, d=duration (milliseconds)
//
H4P_sirenBuzz(p,d);
H4P_sirenChirp(p,d);
H4P_sirenHiLo(p,d);
H4P_sirenScreech(p,d);
H4P_sirenWoopWoop(p,d);
```

[Example Sketch](../examples/MUSIC/H4P_Sirens/H4P_Sirens.ino)

```cpp
//
// Voices (one for each "track" or part to be played simultaneously)
//
// Constrcutor:
// 
H4P_Voice(uint8_t pin);
void chromaticRun(const string& nStart,const string& nFinish,const char duration); // play every semitone between nStart note and nFinish note using note name duration e.g. 'c' = crotchet = 1/4 note
void play(const string& tune,int transpose=0); // play tune transposed +/- N semitones
void rest(const char duration); // silence for note name duration e.g. 'c' = crotchet = 1/4 note

```

[Example Sketch - 'Gymnopedie No. 1' - Erik Satie](../examples/MUSIC/H4P_GymnopedieNo1/H4P_GymnopedieNo1.ino)
[Example Sketch - 'Take on Me' - Aha](../examples/MUSIC/H4P_TakeOnMe/H4P_TakeOnMe.ino)

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
