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
#include<H4P_FlasherController.h>

#ifdef H4FC_MORSE_SUPPORT 
std::unordered_map<char,string>	H4P_FlasherController::_morse={
	{'a',".-"},
	{'b',"-..."},
	{'c',".-.-"},
	{'d',"-.."},
	{'e',"."},
	{'f',"..-."},
	{'g',"--."},
	{'h',"...."},
	{'i',".."},
	{'j',".---"},
	{'k',"-.-"},
	{'l',".-.."},
	{'m',"--"},
	{'n',"-."},
	{'o',"---"},
	{'a',"..."},
	{'p',".--."},
	{'q',"--.-"},
	{'r',".-."},
	{'s',"..."},
	{'t',"-"},
	{'u',"..-"},
	{'v',"...-"},
	{'w',".--"},
	{'x',"-..-"}, 
	{'y',"-.--"},
	{'z',"--.."}
};
#endif /////////////////////////////////////// _MORSE_SUPPORT
//
//      H4P_FlasherController
//
void H4P_FlasherController::_dynaLoad(uint8_t pin,H4GM_SENSE active,H4FC_FN_F1 f1,H4FC_FN_F2 f2){
    auto fp=_flashMap[pin];
    if(fp){
        fp->stop();
        f1(fp);
    }
    else {
        auto opp=reinterpret_cast<OutputPin*>(_pGPIO->isManaged(pin));
        if(opp && opp->style==H4GM_PS_OUTPUT) _flashMap[pin]=f2(opp);
        else {
            opp=_pGPIO->Output(pin,active,OFF);
            _dynaLoad(pin,active,f1,f2);
        }
    }
}

void H4P_FlasherController::_flash(uint32_t period,uint8_t duty,uint8_t pin,H4GM_SENSE active){
    // cancel any existing!!!
    stopLED(pin);
	if(duty < 100){ // sanity
        _dynaLoad(pin,active,
            [period,duty](H4Flasher* fp){ fp->PWM(period,duty); },
            [period,duty,this](OutputPin* opp){ return new H4Flasher(opp,period,duty); }
        );
	}
}

void H4P_FlasherController::_hookIn() { _pGPIO=h4prequire<H4P_GPIOManager>(this,H4PID_GPIO); }

void H4P_FlasherController::flashPattern(const char* _pattern,uint32_t _timebase,uint8_t pin,H4GM_SENSE active){
    // cancel any existing!!!
    stopLED(pin);
    _dynaLoad(pin,active,
        [_pattern,_timebase](H4Flasher* fp){ fp->flashPattern(_pattern, _timebase); },
        [_pattern,_timebase,this](OutputPin* opp){ return new H4Flasher(opp,_pattern,_timebase); }
    );
}

void H4P_FlasherController::flashLED(uint32_t period,uint8_t pin,H4GM_SENSE active){ _flash(period*2,50,pin,active); }// simple symmetric SQ wave on/off

void H4P_FlasherController::flashMorse(const char* _pattern,uint32_t _timebase,uint8_t pin,H4GM_SENSE active){// flash arbitrary pattern ... --- ... convert dot / dash into Farnsworth Timing
	string ms;
	vector<string> sym=split(_pattern," ");
	for(auto const& s:sym) {
		for(auto const& c:s) ms+=c == '.' ? "10":"1110";
		ms+="00";
	}
	ms+="00000";
	flashPattern(CSTR(ms),_timebase,pin,active);
}

#ifdef H4FC_MORSE_SUPPORT
void H4P_FlasherController::flashMorseText(const char* letters,uint32_t timebase,uint8_t pin,H4GM_SENSE active){
	string ditdah;
	for(auto const& c:string(letters)) {
		char lc=tolower(c);
		ditdah+=_morse.count(lc) ? _morse[lc]+" ":" ";
	}
	flashMorse(CSTR(ditdah),timebase,pin,active);
}
#endif

void H4P_FlasherController::flashPWM(uint32_t period,uint8_t duty,uint8_t pin,H4GM_SENSE active){ _flash(period,duty,pin,active);	}// flash "PWM"

bool H4P_FlasherController::isFlashing(uint8_t pin){ return _flashMap.count(pin); }

void H4P_FlasherController::pulseLED(uint32_t period,uint8_t pin,H4GM_SENSE active){ _flash(period,0,pin,active); }

void H4P_FlasherController::stopLED(uint8_t pin){
	if(_flashMap.count(pin)) {
        auto fp=_flashMap[pin];
		fp->stop();
		delete fp;
		_flashMap.erase(pin);
	}
}    
//
//      H4Flasher
//
H4Flasher::H4Flasher(OutputPin* opp,uint32_t period,uint8_t duty): _opp(opp){ PWM(period,duty); }

H4Flasher::H4Flasher(OutputPin* opp,const char* pattern,uint32_t timebase): _opp(opp){ flashPattern(pattern,timebase); }

void H4Flasher::_pulse(uint32_t width) {
	_opp->toggle();
	_off=h4.once(width,[this](){ _opp->toggle(); },nullptr,H4P_TRID_PP1x);
}

void H4Flasher::PWM(uint32_t period,uint8_t duty){
	if(duty){
		stop();
		uint32_t width=(duty*period)/100;
		_pulse(width); 
		_timer=h4.every(period,[this,width](){ _pulse(width); },nullptr,H4P_TRID_PWM1);
	} else _pulse(period);
}

void H4Flasher::flashPattern(const char* pattern,uint32_t timebase){
	stop();
    _dots=pattern;
	_timer=h4.every(timebase,[this](){
        _opp->logicalWrite(_dots.front() - 0x30);
		rotate(_dots.begin(),++_dots.begin(),_dots.end());
	},nullptr,H4P_TRID_PATN);
}

void H4Flasher::stop(){
	h4.cancel({_timer,_off});
	_opp->logicalWrite(OFF); // OFF
}