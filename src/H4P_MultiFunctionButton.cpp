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

#include<H4P_WiFiSelect.h>
#include<H4P_WiFi.h>
#include<H4P_AsyncMQTT.h>

void H4P_MultiFunctionButton::progress(H4GPIOPin* ptr){ // run this as each stage changes
    H4GM_PIN(Multistage); // Create the correct pointer type in 'pin'
    switch(pin->stage){
        case 1: // over 2 seconds, slow flash
            h4fc.flashLED(H4MF_SLOW,_led,_active);     
            break;
        case 2: // over 5 seconds, medium flash
            h4fc.flashLED(H4MF_MEDIUM,_led,_active);     
            break;
        case 3: // over 10 seconds, medium flash
            h4fc.flashLED(H4MF_FAST,_led,_active);     
            break;
        default: // do nothing if less than 2 seconds
            break;
    }
}

void H4P_MultiFunctionButton::_start(){
    if(isLoaded(wifiTag()) && isLoaded(winkTag())){
        if(WiFi.getMode() & WIFI_AP) h4fc.flashPWM(1000,10,_led,_active); 
        else h4fc.stopLED(_led); 
    }
    _upHooks();
}

void H4P_MultiFunctionButton::_stop(){
    if(isLoaded(wifiTag()) && isLoaded(winkTag())){
        if(WiFi.getMode() & WIFI_AP) h4fc.stopLED(_led); 
        else h4fc.flashMorse("... --- ...   ",H4MF_TIMEBASE,_led,_active);
    }
    _downHooks();
}

void H4P_MultiFunctionButton::_hookIn(){
    REQUIREBT;
    HOOK_IF_LOADED(wifi);
    DEPEND(wink);
    if(isLoaded(mqttTag())){
        h4mqtt.hookConnect([this](){ h4fc.stopLED(_led); });
        h4mqtt.hookDisconnect([this](){ h4fc.flashPattern("10100000",H4MF_TIMEBASE,_led,_active); });
    }
}

H4P_MultiFunctionButton::H4P_MultiFunctionButton(uint8_t pin,uint8_t mode,H4GM_SENSE b_sense,uint32_t dbTimeMs,uint8_t led,H4GM_SENSE l_sense):
        _led(led),_active(l_sense),H4Plugin(mfnbTag()){
    h4gm.pinFactory<MultistagePin>(false,pin,mode,H4GM_PS_MULTISTAGE,b_sense,dbTimeMs,_sm,[this](H4GPIOPin* ptr){ progress(ptr); });
}
