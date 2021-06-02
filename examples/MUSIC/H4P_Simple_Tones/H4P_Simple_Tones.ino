#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, larger Q size for scheduling, SerialCmd autostop

H4P_ToneController h4tc;

H4P_Voice vox(D8);

uint32_t  T;
std::string fsharp_d="F#3d "; // demissemiquaver, dotted = D
std::string fsharp_s="F#3S "; // dotted semiquaver, normal = s
std::string fsharp_c="F#3c "; // crotchet = 1/4 note = 250mS @ 60 bpm

void h4setup(){
  Serial.printf("You should now hear 'Concert A'  (440Hz) for5 seconds \n",_HAL_freeHeap());
  h4tc.tone(D8,440,5000,4); // 440Hz = Concert 'A', 5sec vol 4
  h4.once(7000,[]{
    h4tc.metronome(60); // 60 BPM
    Serial.println("Some note lengths @ 60bpm:");
    T=h4tc.length(fsharp_d);
    Serial.print("d=");Serial.println(T);
    vox.play(fsharp_d);
    h4.once(T+1000,[]{ // after previous note has finished, + 1 second
      T=h4tc.length(fsharp_s);
      Serial.print("S=");Serial.println(T);
      vox.play(fsharp_s);
      h4.once(T+1000,[]{
        T=h4tc.length(fsharp_c);
        Serial.print("c=");Serial.println(T);
        vox.play(fsharp_c);
        h4.once(T+1000,[]{
          Serial.println("Here's the whole range from GN2 to BN7");
          vox.chromaticRun("GN2","BN7",'d'); // whole range
          Serial.println("After BN7, only dogs can hear it :)");
        });
      });
    });
  }); 
}
