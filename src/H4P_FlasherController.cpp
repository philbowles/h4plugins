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
H4P_FlasherController::H4P_FlasherController(){
        _pid=winkTag(); 
        _names={
            {H4P_TRID_PP1x,uppercase(winkTag())},
            {H4P_TRID_PATN,"PATN"},
            {H4P_TRID_PWM1,"PWM1"}
        };
        _cmds={};
}

void H4P_FlasherController::_flash(uint32_t period,uint8_t duty,uint8_t pin,uint8_t active){
	if(duty < 100){
		h4fc.stopLED(pin);
		_flashMap[pin]=new H4Flasher(pin,period,duty,active);
	}
}

void H4P_FlasherController::flashLED(uint32_t period,uint8_t pin,uint8_t active){ _flash(period*2,50,pin,active); }// simple symmetric SQ wave on/off

void H4P_FlasherController::flashMorse(const char* _pattern,uint32_t _timebase,uint8_t pin,uint8_t active){// flash arbitrary pattern ... --- ... convert dot / dash into Farnsworth Timing
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
void H4P_FlasherController::flashMorseText(const char* letters,uint32_t timebase,uint8_t pin,uint8_t active){
	string ditdah;
	for(auto const& c:string(letters)) {
		char lc=tolower(c);
		ditdah+=_morse.count(lc) ? _morse[lc]+" ":" ";
	}
	flashMorse(CSTR(ditdah),timebase,pin,active);
}
#endif

void H4P_FlasherController::flashPattern(const char* _pattern,uint32_t _timebase,uint8_t pin,uint8_t active){// flash arbitrary pattern 10000111000110
    stopLED(pin);
    _flashMap[pin]=new H4Flasher(pin,_pattern,_timebase,active);
}

void H4P_FlasherController::flashPWM(uint32_t period,uint8_t duty,uint8_t pin,uint8_t active){ _flash(period,duty,pin,active);	}// flash "PWM"

bool H4P_FlasherController::isFlashing(uint8_t pin){ return _flashMap.count(pin); }

void H4P_FlasherController::pulseLED(uint32_t period,uint8_t pin,uint8_t active){ _flash(period,0,pin,active); }

void H4P_FlasherController::stopLED(uint8_t pin){
	if(_flashMap.count(pin)) {
		_flashMap[pin]->stop();
		delete _flashMap[pin];
		_flashMap.erase(pin);
	}
}    
//
//      H4Flasher
//
H4Flasher::H4Flasher(uint8_t pin,uint32_t period,uint8_t duty,uint8_t active): _pin(pin),_active(active){ 
    autoOutput();
    PWM(period,duty);
}

H4Flasher::H4Flasher(uint8_t pin,const char* pattern,uint32_t timebase,uint8_t active): _pin(pin),_active(active){ 
    autoOutput();
    flashPattern(timebase,pattern); 
}

void H4Flasher::autoOutput(){ if(H4Plugin::isLoaded(gpioTag())) h4gm.Output(_pin,a ? ACTIVE_HIGH:ACTIVE_LOW,OFF); }

void H4Flasher::_toggle() { digitalWrite(_pin,!digitalRead(_pin)); }

void H4Flasher::_pulse(uint32_t width) {
	_toggle();
	_off=h4.once(width,[this](){ _toggle(); },nullptr,H4P_TRID_PP1x);
}

void H4Flasher::PWM(uint32_t period,uint8_t duty){
	ms="";t=0;p=period;d=duty;
	if(duty){
		stop();
		uint32_t width=(duty*period)/100;
		_pulse(width); 
		_timer=h4.every(period,bind([this](int width){ _pulse(width); },width),nullptr,H4P_TRID_PWM1);
	} else _pulse(period);
}

void H4Flasher::flashPattern(uint32_t _base,const char* _pattern){
	ms=_pattern;t=_base;p=d=0;
	stop();
	ms=_pattern;
	_timer=h4.every(_base,[this]( ){
		static char prev='0';
		char c=ms.front();
		if(c!=prev) {
		  _toggle();
		  prev=c;
		}
		rotate(ms.begin(),++ms.begin(),ms.end());
	},nullptr,H4P_TRID_PATN);
}
			
void H4Flasher::stop(){
	_timer=h4.cancel(_timer);
	_off=h4.cancel(_off);
	digitalWrite(_pin,!_active); // OFF
}
