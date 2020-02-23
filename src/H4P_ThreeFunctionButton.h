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
#ifndef H4P_ThreeFunctionButton_HO
#define H4P_ThreeFunctionButton_HO

/*
#ifndef LED_BUILTIN
    #define LED_BUILTIN 2
#endif
*/

#include<H4PCommon.h>
#include<H4P_GPIOManager.h>

#include<H4P_BinaryThing.h>

#ifndef H4P_NO_WIFI

extern void h4FactoryReset();

class H4P_ThreeFunctionButton: public H4Plugin{
            H4P_BinaryThing*    _btp;
            uint8_t             _led;
            H4GM_SENSE          _active;
            H4GM_STAGE_MAP _sm={
                {0,[this](H4GPIOPin* ptr){ _btp->_turn(!_btp->state(),_pid); }},
                {H43F_REBOOT,[](H4GPIOPin* ptr){ h4reboot(); }},
                {H43F_FACTORY,[](H4GPIOPin* ptr){ h4FactoryReset(); }}
            };    
            H4_FN_VOID      _createMS;

            void            _hookIn() override;

            void progress(H4GPIOPin* ptr);
    public:
        H4P_ThreeFunctionButton(
            H4P_BinaryThing* btp,  //
            uint32_t dbTimeMs, // arbitrary
//          the input button            
            uint8_t pin,
            uint8_t mode,
            H4GM_SENSE b_sense,
//          the linked LED for message flashing
            uint8_t led,
            H4GM_SENSE l_sense);
};

extern __attribute__((weak)) H4P_ThreeFunctionButton h43fb;

#endif
#endif // H4P_ThreeFunctionButton_H