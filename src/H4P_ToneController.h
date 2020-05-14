/*
 MIT License

Copyright (c) 2020 Phil Bowles <H48266@gmail.com>
   github     https://github.com/philbowles/H4
   blog       https://8266iot.blogspot.com
   groups     https://www.facebook.com/groups/esp8266questions/
              https://www.facebook.com/H4-Esp8266-Firmware-Support-2338535503093896/


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
#ifndef H4P_ToneController_H
#define H4P_ToneController_H

#include<H4PCommon.h>
#include<map>

enum H4P_SIREN {
    H4P_SIREN_BUZZ,
    H4P_SIREN_CHIRP,
    H4P_SIREN_HILO,
    H4P_SIREN_SCREECH,
    H4P_SIREN_WOOPWOOP,
    H4P_SIREN_MAX
};

class H4P_Voice;

using H4P_STAVE         = pair<H4P_Voice&,const string&>;
using H4P_TUNE          = vector<H4P_STAVE>;

class H4P_ToneController: public H4Plugin {
        friend class H4P_Voice;
        VSCMD(_siren);
//      dsqcmb
        static unordered_map<char,uint32_t> timing;
        static unordered_map<string,uint32_t> notes;
        static unordered_map<uint32_t,pair<uint32_t,string>> sirens;
        static vector<uint32_t> xpose;

        static  string      _cleanTune(const string& tune){ return replaceAll(replaceAll(tune,"     ",""),"|",""); }
        static  void        _repeat(const string& siren,uint8_t pin,uint32_t bpm,uint32_t duration);
        static  uint32_t    _transpose(const string& note,int transpose){ return note[0]=='R' ? 0:xpose[notes[note]+transpose]; }
    public:
        H4P_ToneController(uint32_t tempo=60);
        static uint32_t     length(const string& tune);
        static void         metronome(uint32_t tempo);
        static void         multiVox(H4P_TUNE tune,uint32_t tempo,int transpose=0);
        static string       setVolume(const string& s,uint8_t volume);
        static void         siren(H4P_SIREN S,uint8_t pin,uint32_t duration);
        static void         tone(uint8_t pin,uint32_t freq,uint32_t duration=0,uint8_t volume=8);
};

class H4P_Voice {
        friend class H4P_ToneController;
                uint8_t         _pin;

                void        _decompose(const string& note,int transpose,H4_FN_VOID chain);
                void        _play(const string& tune,int transpose,H4_FN_VOID chain);
                void        _tone(uint32_t f,uint8_t effect,uint32_t d,H4_FN_VOID chain);
    public:
        H4P_Voice(uint8_t pin): _pin(pin){ pinMode(pin,OUTPUT); }
        void         chromaticRun(const string& nStart,const string& nFinish,const char duration);
        void         play(const string& tune,int transpose=0);
        void         rest(const char duration){ play(string("R  ").append(1,duration).append(1,' ')); }
};

extern __attribute__((weak)) H4P_ToneController h4tc;

#define H4P_sirenBuzz(p,d) h4tc.siren(H4P_SIREN_BUZZ,p,d)
#define H4P_sirenChirp(p,d) h4tc.siren(H4P_SIREN_CHIRP,p,d)
#define H4P_sirenHiLo(p,d) h4tc.siren(H4P_SIREN_HILO,p,d)
#define H4P_sirenScreech(p,d) h4tc.siren(H4P_SIREN_SCREECH,p,d)
#define H4P_sirenWoopWoop(p,d) h4tc.siren(H4P_SIREN_WOOPWOOP,p,d)

#endif // H4P_ToneController_H
