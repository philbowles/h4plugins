/*
 MIT License

Copyright (c) 2019 Phil Bowles <H48266@gmail.com>
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
#ifndef H4P_ExternalSqWave_HO
#define H4P_ExternalSqWave_HO

#include<H4PCommon.h>
#include<SoftwareSerial.h>

// https://github.com/plerup/espsoftwareserial

class H4P_ExternalSqWave: public H4Plugin, public SoftwareSerial{
//
        uint32_t        __set(vector<string> vs,uint32_t c,uint32_t lim);
        void            __send(uint32_t c,uint32_t v);
        uint32_t        __sweep(vector<string> vs,uint32_t c,uint32_t lim);
//
        VSCMD(_fset);
        VSCMD(_dset);
        VSCMD(_fsweep);
        VSCMD(_dsweep);
    //
        function<void(uint32_t)> _default=[](uint32_t f){ Serial.print("F now ");Serial.println(f); }; // should be reply?

        void            _Sweep(uint32_t c,uint32_t lim,uint32_t timer,uint32_t fromF,uint32_t toF,uint32_t inc,function<void(uint32_t)> onChange);

    public:
        H4P_ExternalSqWave(uint8_t rx,uint8_t tx,uint32_t initialF=0,uint32_t initialD=0);                

        void            dSet(uint32_t d){ __send('D',constrain(d,1,H4ESW_MAX_D)); }
        void            dSweep(uint32_t timer,uint32_t fromF,uint32_t toF,uint32_t inc,function<void(uint32_t)> onChange=[](uint32_t){});
        void            fSet(uint32_t f){ __send('F',constrain(f,1,H4ESW_MAX_F)); }
        void            fSweep(uint32_t timer,uint32_t fromF,uint32_t toF,uint32_t inc,function<void(uint32_t)> onChange=[](uint32_t){});
        void            stop(){ h4.cancelSingleton(H4P_TRID_SQWV); }
};

extern __attribute__((weak)) H4P_ExternalSqWave h4esw;

#endif // H4P_ExternalSqWave_H