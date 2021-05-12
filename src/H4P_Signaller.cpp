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
#include<H4P_Signaller.h>

#ifdef H4FC_MORSE_SUPPORT 
std::unordered_map<char,string>	H4P_Signaller::_morse={
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
//      H4P_Signaller
//
#if H4P_CMDLINE_FLASHERS

uint32_t H4P_Signaller::__validator(vector<string> vs,H4_FN_MSG f){
    return _guard1(vs,[=](vector<string> V){
        auto vs=split(V.back(),",");
        if(vs.size()<3) return H4_CMD_TOO_FEW_PARAMS;
        if(vs.size()>4) return H4_CMD_TOO_MANY_PARAMS;
        if(!stringIsNumeric(vs[0])) return H4_CMD_NOT_NUMERIC;
        if(!stringIsNumeric(vs[1])) return H4_CMD_NOT_NUMERIC;
        if(PARAM_INT(1) > 1) return H4_CMD_OUT_OF_BOUNDS;
        if(!stringIsNumeric(vs[2])) return H4_CMD_NOT_NUMERIC;
        uint8_t p=PARAM_INT(0);
        if(H4P_PinMachine::isManaged(p) && H4P_PinMachine::isOutput(p)) return H4_CMD_NOT_NOW; /// just is op?
        return static_cast<H4_CMD_ERROR>(f(vs));
    });
}

uint32_t H4P_Signaller::_morse(vector<string> vs){ 
    return __validator(vs,[=](vector<string> vs){
        if(vs.size()<4) return H4_CMD_TOO_FEW_PARAMS;
        if(!all_of(vs[3].cbegin(), vs[3].cend(), [](char c){ return c=='.' || c=='-' || c==' '; })) return H4_CMD_PAYLOAD_FORMAT;
        flashMorse(CSTR(vs[3]),PARAM_INT(2),PARAM_INT(0),static_cast<H4PM_SENSE>(PARAM_INT(1)));
        return H4_CMD_OK;
    });
}

uint32_t H4P_Signaller::_onof(vector<string> vs){ 
    return __validator(vs,[=](vector<string> vs){
        if(vs.size()>3) return H4_CMD_TOO_MANY_PARAMS;
        flashPin(PARAM_INT(2),PARAM_INT(0),static_cast<H4PM_SENSE>(PARAM_INT(1)));
        return H4_CMD_OK;
    });
}

uint32_t H4P_Signaller::_patn(vector<string> vs){ 
    return __validator(vs,[=](vector<string> vs){
        if(vs.size()<4) return H4_CMD_TOO_FEW_PARAMS;
        if(!all_of(vs[3].cbegin(), vs[3].cend(), [](char c){ return c=='1' || c=='0'; })) return H4_CMD_PAYLOAD_FORMAT;
        flashPattern(CSTR(vs[3]),PARAM_INT(2),PARAM_INT(0),static_cast<H4PM_SENSE>(PARAM_INT(1)));
        return H4_CMD_OK;
    });
}

uint32_t H4P_Signaller::_pwm(vector<string> vs){ 
    return __validator(vs,[=](vector<string> vs){
        if(vs.size()<4) return H4_CMD_TOO_FEW_PARAMS;
        if(!stringIsNumeric(vs[3])) return H4_CMD_NOT_NUMERIC;
        if(PARAM_INT(3) < 0 || PARAM_INT(3) > 100) return H4_CMD_OUT_OF_BOUNDS;
        flashPWM(PARAM_INT(2),PARAM_INT(3),PARAM_INT(0),static_cast<H4PM_SENSE>(PARAM_INT(1)));
        return H4_CMD_OK;
    });
}

uint32_t H4P_Signaller::_stop(vector<string> vs){ 
    return _guard1(vs,[=](vector<string> vs){
        if(!vs.size()) return H4_CMD_TOO_FEW_PARAMS;
        if(vs.size()>1) return H4_CMD_TOO_MANY_PARAMS;
        if(lowercase(vs[0])==allTag()) stopAll();
        else {
            if(!stringIsNumeric(vs[0])) return H4_CMD_NOT_NUMERIC;
            uint8_t p=PARAM_INT(0);
            if(!isFlashing(p)) return H4_CMD_NOT_NOW;
            stopPin(p);
        } 
        return H4_CMD_OK;
    });
}
#endif

void H4P_Signaller::_dynaLoad(uint8_t pin,H4PM_SENSE active,uint8_t col,H4FC_FN_F1 f1,H4FC_FN_F2 f2){
    auto opp=static_cast<h4pOutput*>(H4P_PinMachine::isManaged(pin));
    if(opp && opp->isOutput()) h4pFlashMap[pin]=f2(opp);
    else {
        new h4pOutput(pin,active,OFF,col);
        _dynaLoad(pin,active,col,f1,f2);
    }
}

void H4P_Signaller::_flash(uint32_t period,uint8_t duty,uint8_t pin,H4PM_SENSE active,uint8_t col){
    stopPin(pin);
	if(duty < 100){
        _dynaLoad(pin,active,col,
            [](H4Flasher* fp){ fp->PWM(); },
            [period,duty](h4pOutput* opp){ return new H4Flasher(opp,period,duty); }
        );
	}
}

void H4P_Signaller::_handleEvent(const string& svc,H4PE_TYPE t,const string& msg){
    if(t==H4PE_SIGNAL){
    #if H4P_ASSUMED_LED
        if(msg.size()){
            vector<string> parts=split(msg,",");
            flashMorse(CSTR(parts[1]),STOI(parts[0]),H4P_ASSUMED_LED,H4P_ASSUMED_SENSE);
        } else stopPin(H4P_ASSUMED_LED);
    #else
        YEVENT(H4PE_SYSWARN,"NO SIGNAL LED DEFINED");
    #endif
    }    
}

void H4P_Signaller::flashMorse(const char* pattern,uint32_t timebase,uint8_t pin,H4PM_SENSE active,uint8_t col){// flash arbitrary pattern ... --- ... convert dot / dash into Farnsworth Timing
	string ms;
	vector<string> sym=split(pattern," ");
	for(auto const& s:sym) {
		for(auto const& c:s) ms+=c == '.' ? "10":"1110";
		ms+="00";
	}
	ms+="00000";
	flashPattern(CSTR(ms),timebase,pin,active,col);
}
void H4P_Signaller::flashMorse(const char* pattern,uint32_t timebase,h4pOutput* p){ flashMorse(pattern,timebase,p->_p,p->_s,p->_c); }

#ifdef H4FC_MORSE_SUPPORT
void H4P_Signaller::flashMorseText(const char* letters,uint32_t timebase,uint8_t pin,H4PM_SENSE active,uint8_t col){
	string ditdah;
	for(auto const& c:string(letters)) {
		char lc=tolower(c);
		ditdah+=_morse.count(lc) ? _morse[lc]+" ":" ";
	}
	flashMorse(CSTR(ditdah),timebase,pin,active,col);
}
void H4P_Signaller::flashMorseText(const char* letters,uint32_t timebase,h4pOutput* p){ flashMorseText(letters,timebase,p->_p,p->_s,p->_c); }
#endif

void H4P_Signaller::flashPattern(const char* pattern,uint32_t timebase,uint8_t pin,H4PM_SENSE active,uint8_t col){
    stopPin(pin);
    _dynaLoad(pin,active,col,
        [](H4Flasher* fp){ fp->flashPattern(); },
        [pattern, timebase](h4pOutput* opp){ return new H4Flasher(opp,pattern,timebase); }
    );
}
void H4P_Signaller::flashPattern(const char* pattern,uint32_t timebase,h4pOutput* p){ flashPattern(pattern,timebase,p->_p,p->_s,p->_c); }

void H4P_Signaller::flashPin(uint32_t period,uint8_t pin,H4PM_SENSE active,uint8_t col){ _flash(period*2,50,pin,active,col); }// simple symmetric SQ wave on/off

void H4P_Signaller::flashPin(uint32_t period,h4pOutput* p){ flashPin(period,p->_p,p->_s,p->_c); }

void H4P_Signaller::flashPWM(uint32_t period,uint8_t duty,uint8_t pin,H4PM_SENSE active,uint8_t col){ _flash(period,duty,pin,active,col);	}// flash "PWM"

void H4P_Signaller::flashPWM(uint32_t period,uint8_t duty,h4pOutput* p){ _flash(period,duty,p->_p,p->_s,p->_c); }

#if H4P_LOG_MESSAGES
void H4P_Signaller::info(){
    H4Service::info();
    if(h4pFlashMap.size()){
        reply(" Flashers");
        for(auto const& f:h4pFlashMap) reply("  pin %d %s",f.first,CSTR(f.second->_dump()));
    }
}
#endif

bool H4P_Signaller::isFlashing(uint8_t pin){ return h4pFlashMap.count(pin); }

void H4P_Signaller::pulsePin(uint32_t period,uint8_t pin,H4PM_SENSE active,uint8_t col){ _flash(period,0,pin,active,col); }

void H4P_Signaller::pulsePin(uint32_t period,h4pOutput* p){ _flash(period,0,p->_p,p->_s,p->_c); }

void H4P_Signaller::stopAll(){
    H4P_FLASHMAP tmp=h4pFlashMap; // else we will potentially delete from interated map
    for(auto const& f:tmp) stopPin(f.first);
}

void H4P_Signaller::stopPin(uint8_t pin){
	if(h4pFlashMap.count(pin)) {
        auto fp=h4pFlashMap[pin];
		fp->stop();
        delete fp;
        h4pFlashMap.erase(pin);
	}
}
void H4P_Signaller::stopPin(h4pOutput* p){ stopPin(p->_p); }

//
//      H4Flasher
//
H4Flasher::H4Flasher(h4pOutput* opp,uint32_t period,uint8_t duty): _period(period),_duty(duty),_opp(opp){ PWM(); }

H4Flasher::H4Flasher(h4pOutput* opp,const char* pattern,uint32_t timebase): _pattern(pattern),_timebase(timebase),_opp(opp){ flashPattern(); }

void H4Flasher::_pulse(uint32_t width) {
	_opp->toggle();
	_off=h4.once(width,[=](){ _opp->toggle(); },nullptr,H4P_TRID_PP1x);
}

void H4Flasher::PWM(){
	if(_duty){
		stop();
		uint32_t width=(_duty*_period)/100;
		_pulse(width); 
		_timer=h4.every(_period,[this,width](){ _pulse(width); },nullptr,H4P_TRID_PWM1);
	} else _pulse(_period);
}

void H4Flasher::flashPattern(){
	stop();
    _dots=_pattern;
	_timer=h4.every(_timebase,[this](){
        _opp->logicalWrite(_dots.front() - 0x30);
		rotate(_dots.begin(),++_dots.begin(),_dots.end());
	},nullptr,H4P_TRID_PATN);
}


#ifdef ARDUINO_ARCH_ESP8266
void H4Flasher::stop(){
	h4.cancel({_timer,_off});
    analogWrite(_opp->_p,H4P_ANALOG_MAX);
	_opp->turn(OFF);
}

H4Flasher::H4Flasher(h4pOutput* opp,uint32_t period,uint32_t valley): _period(period),_valley(valley),_opp(opp){ throb(); }

void H4Flasher::throb(){
    vector<uint32_t> plan;
    uint32_t nSlices=_period/(H4PM_GRANULARITY *2);
    uint32_t thickness=(H4P_ANALOG_MAX - ((H4P_ANALOG_MAX * _valley) / 100)) / nSlices;
    for(int i=0;i<(1+nSlices);i++) plan.push_back(H4P_ANALOG_MAX - (i * thickness));
    vector<uint32_t> tr=plan;
    plan.insert(plan.end(), tr.rbegin(), tr.rend());

    _opp->turn(ON);
    _timer=h4.every(H4PM_GRANULARITY,[=]{
        static int i=0;
        analogWrite(_opp->_p,plan[i++]);
        if(i==plan.size()) i=0;
    },nullptr,H4P_TRID_THRB);
}

void H4P_Signaller::throbPin(uint32_t rate, uint32_t valley, uint8_t pin,H4PM_SENSE active,uint8_t col){
    stopPin(pin);
    _dynaLoad(pin,active,col,
        [=](H4Flasher* fp){ fp->throb(); },
        [rate,valley](h4pOutput* opp){ return new H4Flasher(opp,rate,valley); }
    );
}

void H4P_Signaller::throbPin(uint32_t rate, uint32_t valley,h4pOutput* p){ throbPin(rate,valley,p->_p,p->_s,p->_c); }
#else
void H4Flasher::stop(){
	h4.cancel({_timer,_off});
	_opp->turn(OFF);
}
#endif