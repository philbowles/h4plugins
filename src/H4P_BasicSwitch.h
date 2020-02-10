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
#ifndef H4P_BasicSwitch_HO
#define H4P_BasicSwitch_HO

#include<H4PCommon.h>
#include<H4P_GPIOManager.h>
#include<H4P_MQTT.h>
#ifndef H4P_NO_WIFI

class H4P_BasicSwitch: public H4Plugin{
//
    protected:
            VSCMD(_switch);

        void            _publish(bool b){  
            if(h4._hasName(H4P_TRID_MQTT)) h4mqtt.publishDevice(statetag(),b);
        }

        virtual void    _hookIn() override {
            if(h4._hasName(H4P_TRID_MQTT)) {
                h4mqtt.hookConnect([this](){ _publish(_pp->state); });
            }
        }            
        OutputPin*      _pp;

    public:
        H4P_BasicSwitch(uint8_t pin,H4GM_SENSE sense, uint8_t initial,H4BS_FN_SWITCH f=[](bool){});

            void        turnOff(){ turn(false); }
            void        turnOn(){ turn(true); }
            void        toggle(){ turn(!_pp->state); }
            void        turn(bool b);        
};

extern __attribute__((weak)) H4P_BasicSwitch h4bs;

#endif
#endif // H4P_BasicSwitch_H