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
#include<H4P_WiFi.h>

void H4P_BinarySwitch::_hookIn() {
    auto _pGPIO=h4prequire<H4P_GPIOManager>(this,H4PID_GPIO);
    _pp=_pGPIO->Output(_pin,_sense,_initial,[](H4GPIOPin* ptr){});
    H4Plugin::_hookIn();
}

void H4P_ConditionalSwitch::syncCondition(){ if(_pWiFi) _pWiFi->_sendSSE(conditionTag(),CSTR(stringFromInt(_predicate(state())))); }

void H4P_ConditionalSwitch::_hookIn() {
    _pWiFi=h4pisloaded<H4P_WiFi>(H4PID_WIFI);
    if(_pWiFi) _pWiFi->_uiAdd(H4P_UIO_COND,conditionTag(),H4P_UI_BOOL,"",[this]{ return stringFromInt(_predicate(state())); });
    H4P_BinarySwitch::_hookIn();
}

void H4P_ConditionalSwitch::_setState(bool b) { 
    if(_predicate(b)) H4P_BinarySwitch::_setState(b);
    else if(_pWiFi) _pWiFi->uiMessage("Unable: condition disarmed");
}