/*
 MIT License

Copyright (c) 2020 Phil Bowles <h4plugins@gmail.com>
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
#include<H4P_BinarySwitch.h>

void H4P_BinarySwitch::_init() {
    _pp=new h4pOutput(_pin,_sense,_initial, _color);
    H4P_BinaryThing::_init();
}

void H4P_ConditionalSwitch::_handleEvent(const std::string& svc,H4PE_TYPE t,const std::string& msg){
    switch(t){
        case H4PE_VIEWERS:
            {
                uint32_t mode=STOI(msg);
                if(mode) {
                #if H4P_USE_WIFI_AP
                    if(mode==WIFI_AP) return;
                #endif
                    h4puiAdd(conditionTag(),H4P_UI_BOOL,"o",stringFromInt(_predicate()),H4P_UILED_BI);
                }
            }
    }
    H4P_BinarySwitch::_handleEvent(svc,t,msg);
}

void H4P_ConditionalSwitch::syncCondition(){ h4puiSync(conditionTag(),CSTR(stringFromInt(h4punlocked=_predicate()))); }