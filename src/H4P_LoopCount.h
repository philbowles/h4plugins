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
#ifndef H4P_LoopCount_HO
#define H4P_LoopCount_HO

extern __attribute__((weak)) uint32_t h4Nloops=0;

#include<H4PCommon.h>
/*
        This will only work if you edit inc/H4.h in the H4 library to include:

        #define H4_COUNT_LOOPS true

        You will get a slightly better performance if you also include

        #define H4_NO_USERLOOP      // improves performance

        but h4UserLoop will not get called!

*/
class H4P_LoopCount: public H4Plugin {
            uint32_t    _freq;

            void        _start() override {
                h4.every(_freq,[]{
                    Serial.printf("%u\n",h4Nloops);
                    h4Nloops=0;
                },nullptr,H4P_TRID_LOOP,true);
            }

            void        _stop() override { h4.cancelSingleton(H4P_TRID_LOOP); }

    public:
        H4P_LoopCount(uint32_t freq=1000): _freq(freq), H4Plugin("loop"){ h4Nloops=0; }
};

#endif // H4P_LoopCount_H