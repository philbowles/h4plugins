/*
 MIT License

Copyright (c) 2019 Phil Bowles <h4plugins@gmail.com>
   github     https://github.com/philbowles/esparto
   blog       https://8266iot.blogspot.com     
   groups     https://www.facebook.com/groups/esp8266questions/
              https://www.facebook.com/Esparto-Esp8266-Firmware-Support-2338535503093896/
                			  

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
#include<H4P_BasicSwitch.h>
#ifndef H4P_NO_WIFI

uint32_t H4P_BasicSwitch::_switch(vector<string> vs){ return guardInt1(vs,bind(&H4P_BasicSwitch::turn,this,_1)); }

void H4P_BasicSwitch::turn(bool b){ if(_pp->state!=b) _pp->logicalWrite(b); }        
//
H4P_BasicSwitch::H4P_BasicSwitch(uint8_t pin,H4GM_SENSE sense, uint8_t initial,H4BS_FN_SWITCH f){
    if(!h4._hasName(H4P_TRID_GPIO)){
        _pid=onoftag();
        _names={ {H4P_TRID_UBSW,uppercase(_pid)} };
        _cmds={
            {"on",     {H4PC_ROOT, 0, CMD(turnOn)}},
            {"off",    {H4PC_ROOT, 0, CMD(turnOff)}},
            {"switch", {H4PC_ROOT, 0, CMDVS(_switch)}},
            {"toggle", {H4PC_ROOT, 0, CMD(toggle)}}
        };
// move to hookin ? NO
        _pp=h4gm.Output(pin,sense,initial,[f,this](H4GPIOPin* ptr){
            H4GM_PIN(Output);
            _publish(pin->state);
            f(pin->state);
        });
    } 
}
#endif