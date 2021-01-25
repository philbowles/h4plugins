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
#include<H4P_MultiFunctionButton.h>
#include<H4P_FlasherController.h>
#include<H4P_AsyncMQTT.h>

void H4P_MultiFunctionButton::_progress(H4GPIOPin* ptr){ // run this as each stage changes
    H4GM_PIN(Multistage); // Create the correct pointer type in 'pin'
    switch(pin->stage){
        case 1: // over 2 seconds, slow flash
            PLOG("STAGE 1 - will reboot");
            _pSignal->flashLED(H4MF_SLOW,H4P_SIGNAL_LED,H4P_SIGNAL_SENSE);
            break;
        case 2: // over 5 seconds, medium flash
            PLOG("STAGE 2 - will factory reset");
            _pSignal->flashLED(H4MF_MEDIUM,H4P_SIGNAL_LED,H4P_SIGNAL_SENSE);
            break;
        default: // do nothing if less than 2 seconds
            break;
    }
}

void H4P_MultiFunctionButton::_hookIn(){
    _pSignal=h4prequire<H4P_FlasherController>(H4PID_WINK);
    _btp=h4prequire<H4P_BinaryThing>(H4PID_ONOF);
    _pGPIO=h4pdepend<H4P_GPIOManager>(this,H4PID_GPIO);
    H4Plugin::_hookIn();
}

H4P_MultiFunctionButton::H4P_MultiFunctionButton(uint8_t pin,uint8_t mode,H4GM_SENSE b_sense,uint32_t dbTimeMs): H4Plugin(H4PID_MFNB){
    _pGPIO->pinFactory<MultistagePin>(false,pin,mode,H4GM_PS_MULTISTAGE,b_sense,dbTimeMs,_sm,[this](H4GPIOPin* ptr){ _progress(ptr); });
}