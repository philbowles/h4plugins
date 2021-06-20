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
#if H4_COUNT_LOOPS

extern uint32_t h4Nloops;

#include<H4Service.h>
#include<H4P_EmitTick.h>
/*
        This will only work if you edit inc/H4.h in the H4 library to include:

        #define H4_COUNT_LOOPS 1

        You will get a slightly better performance if you also include

        #define H4_USERLOOP     0 // improves performance

        but h4UserLoop will not get called!

*/
class H4P_EmitLoopCount: public H4Service {
        virtual void _handleEvent(const std::string& svc,H4PE_TYPE t,const std::string& msg) override {
                XEVENT(H4PE_LOOPS,"%u",h4Nloops);
                h4Nloops=0;
            };

    public:
        H4P_EmitLoopCount(): H4Service("loop",H4PE_HEARTBEAT){ 
            h4Nloops=0;
            depend<H4P_EmitTick>(tickTag());
        }
};
#else
struct H4P_EmitLoopCount {};
#endif