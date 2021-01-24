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
#pragma once

extern __attribute__((weak)) uint32_t h4Nloops=0;

#include<H4PCommon.h>
#include<H4P_EmitTick.h>
/*
        This will only work if you edit inc/H4.h in the H4 library to include:

        #define H4_COUNT_LOOPS true

        You will get a slightly better performance if you also include

        #define H4_NO_USERLOOP      // improves performance

        but h4UserLoop will not get called!

*/
/*
#if H4_COUNT_LOOPS
    #pragma message("COUNTING LOOPS")
#endif
*/
class H4P_EmitLoopCount: public H4Plugin {
            void _handleEvent(H4PID pid,H4P_EVENT_TYPE t,const string& msg) override {
                PEVENT(H4P_EVENT_LOOPS,"%u",h4Nloops);
                h4Nloops=0;
            };

            void _hookIn() {
                depend<H4P_EmitTick>(this,H4PID_1SEC);
                H4Plugin::_hookIn();
            }

    public:
        H4P_EmitLoopCount(): H4Plugin(H4PID_LOOP,H4P_EVENT_HEARTBEAT){ h4Nloops=0; }
};