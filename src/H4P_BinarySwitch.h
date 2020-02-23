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
#ifndef H4P_BinarySwitch_HO
#define H4P_BinarySwitch_HO

#include<H4P_BinaryThing.h>
#include<H4P_GPIOManager.h>

class H4P_BinarySwitch: public H4P_BinaryThing{
//
    protected:
        OutputPin*          _pp;
        virtual void        _setState(bool b) override { 
            _pp->logicalWrite(_state=b);
        }        
    public:
        H4P_BinarySwitch(uint8_t pin,H4GM_SENSE sense, uint32_t initial,H4BS_FN_SWITCH f=[](bool){}): H4P_BinaryThing(f,initial){
            _pp=h4gm.Output(pin,sense,initial,[](H4GPIOPin* ptr){});
        }
};

//extern __attribute__((weak)) H4P_BinarySwitch h4bs;

#endif // H4P_BinarySwitch_H