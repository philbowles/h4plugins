
/*
 MIT License

Copyright (c) 2020 Phil Bowles <h4plugins@gmail.com>
   github     https://github.com/philbowles/H4P_ToneController
   blog       https://8266iot.blogspot.com     
   groups     https://www.facebook.com/groups/esp8266questions/
              https://www.facebook.com/H4P_ToneController-Esp8266-Firmware-Support-2338535503093896/
                			  

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifdef ARDUINO_ARCH_ESP8266
#include<H4P_ToneController.h>
#include<H4.h>

unordered_map<string,uint32_t> H4P_ToneController::notes={
            {"R  ",0}, // REST

            {"GN2",100},
            {"G#2",104},
            {"AN2",110},
            {"A#2",117},
            {"BN2",123},

            {"CN3",131},
            {"C#3",139},
            {"DN3",147},
            {"D#3",156},
            {"EN3",165},
            {"FN3",175},
            {"F#3",185},
            {"GN3",196},
            {"G#3",208},
            {"AN3",220},
            {"A#3",233},
            {"BN3",247},

            {"CN4",262},
            {"C#4",277},
            {"DN4",294},
            {"D#4",311},
            {"EN4",330},
            {"FN4",349},
            {"F#4",370},
            {"GN4",392},
            {"G#4",415},
            {"AN4",440},
            {"A#4",466},
            {"BN4",494},

            {"CN5",523},
            {"C#5",554},
            {"DN5",587},
            {"D#5",622},
            {"EN5",659},
            {"FN5",698},
            {"F#5",740},
            {"GN5",784},
            {"G#5",831},
            {"AN5",880},
            {"A#5",932},
            {"BN5",988},

            {"CN6",1047},
            {"C#6",1109},
            {"DN6",1175},
            {"D#6",1245},
            {"EN6",1319},
            {"FN6",1397},
            {"F#6",1480},
            {"GN6",1568},
            {"G#6",1661},
            {"AN6",1760},
            {"A#6",1865},
            {"BN6",1976},

            {"CN7",2093},
            {"C#7",2217},
            {"DN7",2349},
            {"D#7",2489},
            {"EN7",2637},
            {"FN7",2794},
            {"F#7",2960},
            {"GN7",3136},
            {"G#7",3322},
            {"AN7",3520},
            {"A#7",3729},
            {"BN7",3951}
        };

vector<uint32_t> H4P_ToneController::xpose;
unordered_map<char,uint32_t> H4P_ToneController::timing;

#define H4P_STAVE_STAGGER 5

unordered_map<uint32_t,pair<uint32_t,string>> H4P_ToneController::sirens={
    {H4P_SIREN_HILO,{50,"GN5q EN4q "}},
    {H4P_SIREN_SCREECH,{750,"CN3d CN7d "}},
    {H4P_SIREN_CHIRP,{400,"BN7d A#7d AN7d G#7d GN7d F#7d FN7d R  M R  M "}},
    {H4P_SIREN_BUZZ,{750,"CN3d R  d "}},
    {H4P_SIREN_WOOPWOOP,{450,"AN3d AN3d AN3d AN3d CN4d CN4d EN4d AN4d BN5d GN5d C#6d R  M "}}
};

H4P_ToneController::H4P_ToneController(uint32_t tempo): H4Plugin("tone"){
    for(auto const &n:notes) xpose.push_back(n.second); // build ordered table of f
    sort(xpose.begin(),xpose.end());
    for(auto &n:notes) {
        uint32_t f=n.second;
        n.second=distance(xpose.begin(), find(xpose.begin(),xpose.end(),f));
    }
    metronome(tempo);

    _cmds={ {_pName,    { H4PC_H4, 0, CMDVS(_siren) }} };
}

void H4P_ToneController::_repeat(const string& siren,uint8_t pin,uint32_t speed,uint32_t duration){
    metronome(speed);       
    H4P_Voice* v=new H4P_Voice(pin);
    uint32_t t=length(siren);
    v->play(siren);
    H4_TIMER h=h4.every(t+H4P_STAVE_STAGGER,[v,siren]{ v->_play(siren,0,[]{}); });
    if(duration) h4.once(duration,[v,h,pin]{ 
        analogWrite(pin,0);
        h4.cancel(h); 
        delete v;
    });
}

uint32_t H4P_ToneController::_siren(vector<string> vs){
    /*
    if(vs.size()<2) return H4_CMD_TOO_FEW_PARAMS;
    if(vs.size()>2) return H4_CMD_TOO_MANY_PARAMS;
    if(!isNumeric(H4PAYLOAD)) return H4_CMD_NOT_NUMERIC;
    if(H4PAYLOAD_INT > H4P_SIREN_MAX) return H4_CMD_OUT_OF_BOUNDS;
*/
    return H4_CMD_OK;
}

uint32_t H4P_ToneController::length(const string& tune){
    uint32_t L=0;
    string t=_cleanTune(tune);
    if(!(t.size()%5)) for(int i=3;i<t.size();i+=5) L+=timing[t[i]];
    return L;
}

void H4P_ToneController::metronome(uint32_t tempo){
    if(tempo){
        uint16_t bpm=(7880/tempo);
        string order="dDsSqQcCmMbB";
        timing['d']=bpm;
        timing['D']=(bpm*3)/2;
        for(int i=2;i<12;i+=2) {
            timing[order[i]]=timing[order[i-2]]*2;
            timing[order[i+1]]=(timing[order[i]]*3)/2;
        }
        /*
        Serial.printf("Tempo=%d\n",tempo);
        for(auto const& o:order) Serial.printf("%c=%d\n",o,timing[o]);
        */
    }
}
void H4P_ToneController::multiVox(H4P_TUNE tune,uint32_t tempo,int transpose){
    metronome(tempo);
    int i=tune.size()-1;
    for(auto const& t:tune) h4.once((H4P_STAVE_STAGGER * (i--)),[t,transpose]{ t.first.play(t.second,transpose); });
}

string H4P_ToneController::setVolume(const string& s,uint8_t volume){
    string t=_cleanTune(s);
    if(!(t.size()%5)){
        for(int i=4;i<t.size();i+=5) t[i]=volume+0x30;
        return t;
    }
    return "";
}

void H4P_ToneController::siren(H4P_SIREN S,uint8_t pin,uint32_t duration){
    pair<uint32_t,string> def=sirens[S];
    _repeat(def.second,pin,def.first,duration);
}

void H4P_ToneController::tone(uint8_t pin,uint32_t freq,uint32_t duration,uint8_t volume){
    H4P_Voice* v=new H4P_Voice(pin);
    v->_tone(freq,duration ? volume+0x30:'-',duration,[v]{ delete v; });
}
//
//      Voice
//
void H4P_Voice::_play(const string& tune,int transpose,H4_FN_VOID chain){
    if(tune.size()){
        _decompose(string(tune.begin(),tune.begin()+5),transpose,[this,tune,transpose,chain](){
            _play(string(tune.begin()+5,tune.end()),transpose,chain);
        });
    } else if(chain) chain();
    else analogWrite(_pin,0);
}

void H4P_Voice::_decompose(const string& n,int transpose,H4_FN_VOID chain){//,uint8_t pin,H4P_NOTE* p){
    string note(n);
    char effect=note.back();note.pop_back();
    char duration=note.back();note.pop_back();
/* 
    Serial.printf("T=%u RAW NOTE P%d %s value=%s d=%c(%d) e=%c f1=%d xp=%d f2=%d\n",
        millis(),
        _pin,
        CSTR(n),
        CSTR(note),
        duration,
        H4P_ToneController::timing[duration],
        effect,
        H4P_ToneController::xpose[H4P_ToneController::notes[note]],
        transpose,
        H4P_ToneController::_transpose(note,transpose));
*/
    if(H4P_ToneController::notes.count(note)) _tone(H4P_ToneController::_transpose(note,transpose),effect,H4P_ToneController::timing[duration],chain);
}

void  H4P_Voice::chromaticRun(const string& nStart,const string& nFinish,const char duration){
    if(H4P_ToneController::notes.count(nStart) && H4P_ToneController::notes.count(nFinish)){
        vector<string> run;
        bool swapped=false;
        uint32_t i1=H4P_ToneController::notes[nStart];
        uint32_t i2=H4P_ToneController::notes[nFinish];
        if(i1>i2) {
            swap(i1,i2);
            swapped=true;
        }
        for(int j=i1;j<=i2;j++){
            auto note=find_if(H4P_ToneController::notes.begin(),H4P_ToneController::notes.end(),[&j](pair<string,uint32_t> const& p){ return j==p.second; });
            run.push_back((string(note->first)).append(1,duration).append(1,'8'));
        }
        if(swapped) reverse(run.begin(),run.end());
        play(join(run,""));
    }
}

void H4P_Voice::play(const string& tune,int transpose){
    string choon=H4P_ToneController::_cleanTune(tune);
    if(!(choon.size()%5)) _play(choon,transpose,[]{});
}

void H4P_Voice::_tone(uint32_t f,uint8_t effect,uint32_t d,H4_FN_VOID chain){
    if(f){ // sound it
        analogWriteFreq(f);
        int8_t fx=effect-0x30;
        if(fx < 0 || fx >8) fx=8;
        analogWrite(_pin,(512/(1 << (9-fx)))-1);
    }
    h4.once(d,[this,effect,chain](){
        if(effect!='-') analogWrite(_pin,0);
        h4.queueFunction(chain); // recurse on main loop!
    });
}
#endif