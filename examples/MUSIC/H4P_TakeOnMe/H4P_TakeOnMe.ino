#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, larger Q size for scheduling, SerialCmd autostop

#define TREBLE  D8
#define BASS    D7

H4P_ToneController h4tc; // defaults to metornome = 60 b.p.m.

H4P_Voice vox_t(TREBLE);
H4P_Voice vox_b(BASS);
//
// Take On Me - Aha
//
// treble clef - melody
string tom_tc="F#4q |F#4q |DN4q |BN3q |R  q |BN3q |R  q |EN4q " \
              "R  q |EN4q |R  q |EN4q |G#4q |G#4q |AN4q |BN4q " \
              "AN4q |AN4q |AN4q |EN4q |R  q |DN4q |R  q |F#4q " \
              "R  q |F#4q |R  q |F#4q |EN4q |EN4q |F#4q |EN4q "; 
// bass clef left-hand accompaniment
string tom_bc="BN2c2|BN3c2|R  q |BN2q2|BN3c2|EN3c2|EN4c2|R  q |EN3q2|EN4c2" \
              "AN2c2|AN3c2|R  q |AN2q2|AN3c2|DN3c2|DN4c2|     |C#3c2|C#4c2"; 
// double up the strings (we play it twice)
string tom_t=tom_tc.append(tom_tc); 
string tom_b=tom_bc.append(tom_bc);

string bLouder=h4tc.setVolume(tom_b,4);  // too quiet as written (vol=2) crank it up to 5

H4P_TUNE  takeOnMe={
    {vox_t,tom_t}, // use voice vox_t to play right-hand melody part (treble clef) on D8
    {vox_b,bLouder} // " vox_b   left hand part (bass clef) on D, but turned up a bit!
};

uint32_t T;

void h4setup(){
  h4tc.metronome(165); // Beats per minute (BPM) 
  T=h4tc.length(tom_t); // musical time length in mS so we knwo when to srat next piece
  Serial.printf("You should now hear the melody of 'Take on Me' by Aha\n");
  vox_t.play(tom_t);
  h4.once(T+5000,[]{ // for a bit of breathing space
    Serial.printf("Now 5 semitones lower...\n");  
    vox_t.play(tom_t,-5);
    h4.once(T+5000,[]{ // for a bit of breathing space
      Serial.printf("Now the left-hand part (a bit too quiet?)...\n");  
      vox_t.play(tom_b,0);
      h4.once(T+5000,[]{ // for a bit of breathing space
        Serial.printf("...let's crank it up to 11! (8 actually)\n");  
        vox_t.play(h4tc.setVolume(tom_b,11));
        h4.once(T+5000,[]{ // for a bit of breathing space
          Serial.printf("Now the melody again at half speed\n"); 
          h4tc.metronome(82); 
          vox_t.play(tom_t);
          h4.once((2*T)+5000,[]{ // for 2x breathing space as tune now at 1/2 speed
            Serial.printf("AND NOW: The whole thing, in stereo\n"); 
            h4tc.multiVox(takeOnMe,165); // 165 B.P.M.
          });
        });
      });
    });
  });
}