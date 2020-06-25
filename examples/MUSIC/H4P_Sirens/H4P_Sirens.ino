#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, larger Q size for scheduling, SerialCmd autostop
#define TONE  D8

H4P_ToneController h4tc;

#define DURATION  10000
/*
#define H4P_sirenBuzz(p,d) h4tc.siren(H4P_SIREN_BUZZ,p,d)
#define H4P_sirenChirp(p,d) h4tc.siren(H4P_SIREN_CHIRP,p,d)
#define H4P_sirenHiLo(p,d) h4tc.siren(H4P_SIREN_HILO,p,d)
#define H4P_sirenScreech(p,d) h4tc.siren(H4P_SIREN_SCREECH,p,d)
#define H4P_sirenWoopWoop(p,d) h4tc.siren(H4P_SIREN_WOOPWOOP,p,d)

 */
void h4setup(){
  Serial.println("Each Siren Will be played for 10 seconds");
  Serial.println("'BUZZ'");
  H4P_sirenBuzz(D8,DURATION);
  h4.once(DURATION+1000,[]{
    Serial.println("'CHIRP'");
    H4P_sirenChirp(D8,DURATION);
    h4.once(DURATION+1000,[]{
      Serial.println("'HI-LO'");
      H4P_sirenHiLo(D8,DURATION);
      h4.once(DURATION+1000,[]{
        Serial.println("'SCREECH'");
        H4P_sirenScreech(D8,DURATION);
        h4.once(DURATION+1000,[]{
          Serial.println("'WOOP-WOOP'");
          H4P_sirenWoopWoop(D8,DURATION);
        });  
      });  
    });  
  });  
}
