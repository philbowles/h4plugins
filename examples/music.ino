#include<string>
#include<vector>
#include<map>

using namespace std;

#define BUZZ  4
std::map<string,uint32_t> notes={
  {"G2S",104},
  {"A2N",110},
  {"A2S",117},
  {"B2N",123},

  {"C3N",131},
  {"C3S",139},
  {"D3N",147},
  {"D3S",156},
  {"E3N",165},
  {"F3N",175},
  {"F3S",185},
  {"G3N",196},
  {"G3S",208},
  {"A3N",220},
  {"A3S",233},
  {"B3N",247},

  {"C4N",262},
  {"C4S",277},
  {"D4N",294},
  {"D4S",311},
  {"E4N",330},
  {"F4N",349},
  {"F4S",370},
  {"G4N",392},
  {"G4S",415},
  {"A4N",440},
  {"A4S",466},
  {"B4N",494},

  {"C5N",523},
  {"C5S",554},
  {"D5N",587},
  {"D5S",622},
  {"E5N",659},
  {"F5N",698},
  {"F5S",740},
  {"G5N",784},
  {"G5S",831},
  {"A5N",880},
  {"A5S",932},
  {"B5N",988},

  {"C6N",1047},
  {"C6S",1109},
  {"D6N",1175},
  {"D6S",1245},
  {"E6N",1319},
  {"F6N",1397},
  {"F6S",1480},
  {"G6N",1568},
  {"G6S",1661},
  {"A6N",1760},
  {"A6S",1865},
  {"B6N",1976},

  {"C7N",2093},
  {"C7S",2217},
  {"D7N",2349},
  {"D7S",2489},
  {"E7N",2637},
  {"F7N",2794},
  {"F7S",2960},
  {"G7N",3136},
  {"G7S",3322},
  {"A7N",3520},
  {"A7S",3729},
  {"B7N",3951}
};

void chromatic(const string& n,uint32_t d=1000){
    string order="CDEFGAB";
    for(auto const& o:order){
        string nat=o+n+"N";
        string sharp=o+n+"S";
        if(notes.count(nat)) play(nat,d);
        if(notes.count(sharp)) play(sharp,d);
    }
}

void play(const string& note,uint32_t d){
  Serial.printf("%s %d\n",note.c_str(),notes[note]);
  analogWriteFreq(notes[note]);
  analogWrite(BUZZ,255); 
  delay(d);
  analogWrite(BUZZ,0); 
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(BUZZ,OUTPUT);
}

void loop() {
  //for(auto const& n:notes) play(n.first,1000);
    chromatic("2");
  delay(1000);
  }
  