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
#include<H4P_SerialCmd.h>
#include<H4P_BinaryThing.h>

void H4P_BinaryThing::_onChange(bool b){
    h4.queueFunction([=](){ _thing(b); });
    auto off=h4p.gvGetInt(autoOffTag());
    if(off) h4.once(off,[=]{ h4p.gvSetInt(stateTag(),OFF); });
}

void H4P_BinaryThing::_handleEvent(const std::string& svc,H4PE_TYPE t,const std::string& msg){ 
    switch(t){
        case H4PE_VIEWERS:
            {
                uint32_t mode=STOI(msg);
                if(mode) {
                #if H4P_USE_WIFI_AP
                    if(mode==WIFI_AP) return;
                #endif
                    h4puiAdd(autoOffTag(),H4P_UI_INPUT,"o");
                    h4puiAdd(stateTag(),H4P_UI_IMGBTN,"o");
                }
            }
            break;
        case H4PE_GVCHANGE:
            if(_running && svc==stateTag()) return _onChange(STOI(msg));
    }
}

void H4P_BinaryThing::autoOff(uint32_t T){ h4p.gvSetInt(autoOffTag(),T); }

void H4P_BinaryThing::svcDown() { 
    h4.cancelSingleton(H4P_TRID_BTTO);
    H4Service::svcDown();
}
//
//      H4P_ConditionalThing
//
void H4P_ConditionalThing::_handleEvent(const std::string& svc,H4PE_TYPE t,const std::string& msg){
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
    H4P_BinaryThing::_handleEvent(svc,t,msg);
}

void H4P_ConditionalThing::syncCondition(){ h4puiSync(conditionTag(),CSTR(stringFromInt(h4punlocked=_predicate()))); }