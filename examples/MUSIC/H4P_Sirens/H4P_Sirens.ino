#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, larger Q size for scheduling, SerialCmd autostop
#define TONE  27

H4P_ToneController tony;

#define DURATION  10000

void h4setup(){
  Serial.println("Each Siren Will be played for 10 seconds");
  Serial.println("'BUZZ'");
  tony.sirenBuzz(TONE,DURATION);
  h4.once(DURATION+1000,[]{
    Serial.println("'CHIRP'");
    tony.sirenChirp(TONE,DURATION);
    h4.once(DURATION+1000,[]{
      Serial.println("'HI-LO'");
      tony.sirenHiLo(TONE,DURATION);
      h4.once(DURATION+1000,[]{
        Serial.println("'SCREECH'");
        tony.sirenScreech(TONE,DURATION);
        h4.once(DURATION+1000,[]{
          Serial.println("'WOOP-WOOP'");
          tony.sirenWoopWoop(TONE,DURATION);
        });  
      });  
    });  
  });  
}