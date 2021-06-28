#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, larger Q size for scheduling, SerialCmd autostop
#define TREBLE  D8
#define BASS    D7
#ifdef ARDUINO_ARCH_ESP32
  #define TREBLE    27
  #define BASS      26
  #define CHORD     25
  #define L_TREBLE  17
  #define L_BASS     5
  #define L_CHORD   18 
  #define FAKE_GND  16 
#else
  #define TREBLE    D7
  #define BASS      D6
  #define CHORD     D5
  #define L_TREBLE  D4
  #define L_BASS    D3
  #define L_CHORD   D2 
#endif
H4P_ToneController h4tc;

H4P_Voice vox_t(TREBLE);
H4P_Voice vox_b(BASS);
H4P_Voice vox_c(CHORD);
H4P_Voice lux_t(L_TREBLE);
H4P_Voice lux_b(L_BASS);
H4P_Voice lux_c(L_CHORD);
//
// Gymnopedie No. 1 - Erik Satie
//                  |                 |                 |                 |                 |                 |                 |
std::string gym_t1="R  m |F#6c5|R  m |EN6c5|R  m |F#6c5|R  m |EN6c5|R  c |F#7c |AN7c |GN7c |F#7c |C#7c |BN5c |C#7c |DN6c |AN6m |     |EN7c |F#6m |     |DN6c5|F#6m |     |EN7c5|F#6m |     |DN6c5|F#6m |     |EN7c5";
std::string gym_b1="R  m |DN6c5|R  m |C#6c5|R  m |DN6c5|R  m |C#6c5|R  c |R  c |DN6c |R  c |R  c |EN6c |R  c |R  c |AN6c |R  c |R  c |C#6c |R  c |R  c |AN6c5|R  c |R  c |C#6c5|R  c |R  c |AN6c5|R  c |R  c |C#6c5"; 
std::string gym_c1="GN3m5|BN5c5|DN3m |AN5c5|GN3m5|BN5c5|DN3m5|AN5c5|GN3m |     |BN5c |DN3m |     |AN5c |GN3m |     |BN5c |DN3m |     |AN5c |GN3m5|     |BN5c5|DN3m5|     |AN5c5|GN3m5|     |BN5c5|DN3m5|     |AN5c5"; 
std::string gym_t2="R  c |F#7c |AN7c |GN7c |F#7c |C#7c |BN5c |C#7c |DN6c |AN6m |     |EN7c |F#6m |     |DN6c |F#6m6|     |EN7c6|F#6m4|     |DN6c4|F#6m2|     |EN7c1";
std::string gym_b2="R  c |R  c |DN6c |R  c |R  c |EN6c |R  c |R  c |AN6c |R  c |R  c |C#6c |R  c |R  c |AN6c |R  c |R  c |C#6c6|R  c |R  c |AN6c4|R  c |R  c |C#6c1"; 
std::string gym_c2="GN3m |     |BN5c |DN3m |     |AN5c |GN3m |     |BN5c |DN3m |     |AN5c |GN3m |     |BN5c |DN3m6|     |AN5c6|GN3m4|     |BN5c4|DN3m3|     |AN5c1"; 

std::string gym_t(gym_t1+gym_t2);
std::string gym_b(gym_b1+gym_b2);
std::string gym_c(gym_c1+gym_c2);

H4P_TUNE  gymnopedie1={
    {vox_t,gym_t}, // use voice vox_t to play right-hand melody part (treble clef) on D8
    {vox_b,gym_b}, // " vox_b   left hand part (bass clef)
    {vox_c,gym_c}, // extra channel for chords
    {lux_t,gym_t}, // use voice vox_t to play right-hand melody part (treble clef) on D8
    {lux_b,gym_b}, // " vox_b   left hand part (bass clef)
    {lux_c,gym_c} // extra channel for chords
};

void h4setup(){
  #ifdef ARDUINO_ARCH_ESP32 // fake a GND for the light show
    pinMode(FAKE_GND,OUTPUT);
    digitalWrite(FAKE_GND,LOW);
  #endif
  Serial.println("Enjoy...");
  h4tc.multiVox(gymnopedie1,85); // 85 B.P.M.  
}