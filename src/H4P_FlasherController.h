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
#ifndef H4FLASHER_HO
#define H4FLASHER_HO

#include<H4PCommon.h>
#include<H4P_GPIOManager.h>

class H4Flasher{
            uint8_t         _active=HIGH;
            H4_TASK_PTR     _timer=nullptr;
            H4_TASK_PTR	    _off=nullptr;

                string 		ms;
                uint32_t	a,t,p,d,v;

                void 		_pulse(uint32_t width);	
                void 		_toggle();

                uint8_t 	_pin;
    public:  
        H4Flasher(uint8_t pin,uint32_t period,uint8_t duty,uint8_t active);
        H4Flasher(uint8_t pin,const char* pattern,uint32_t timebase,uint8_t active);

                void        autoOutput();
                void        flashPattern(uint32_t timebase,const char* pattern);
                void        PWM(uint32_t period,uint8_t duty);
                void        stop();
};

constexpr const char* fTag(){ return "wink"; }

class H4P_FlasherController: public H4Plugin {
        std::unordered_map<uint8_t,H4Flasher*> _flashMap;
        void            _flash(uint32_t period,uint8_t duty,uint8_t pin,uint8_t active=HIGH);
    public:
        H4P_FlasherController();
             
		void 			flashLED(uint32_t rate, uint8_t pin,uint8_t active=HIGH);
		void 			flashMorse(const char *pattern, uint32_t timebase, uint8_t pin,uint8_t active=HIGH);
		
#ifdef H4FC_MORSE_SUPPORT
		static std::unordered_map<char,string> _morse; // tidy
		void 	        flashMorseText(const char * pattern,uint32_t timebase,uint8_t pin,uint8_t active=HIGH);
#endif
		void 			flashPattern(const char * pattern,uint32_t timebase,uint8_t pin,uint8_t active=HIGH);
		void 			flashPWM(uint32_t period,uint8_t duty,uint8_t pin,uint8_t active=HIGH);	
		bool 			isFlashing(uint8_t pin);
		void 			pulseLED(uint32_t period,uint8_t pin,uint8_t active=HIGH);
        void            stopLED(uint8_t pin);				
};

extern __attribute__((weak)) H4P_FlasherController h4fc;

#endif // H4FLASHER_Hs