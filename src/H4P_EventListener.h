/*
 MIT License

Copyright (c) 2020 Phil Bowles <H48266@gmail.com>
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
#pragma once

#include<H4Service.h>

class H4P_EventListener: public H4Service{ 
            H4P_FN_EVENTHANDLER _f;
    protected:
        virtual void _handleEvent(const std::string& s,H4PE_TYPE t,const std::string& m) override { if(_running) _f(s,t,m); }
    public: 
        H4P_EventListener(uint32_t filter,H4P_FN_EVENTHANDLER f): _f(f), H4Service("ears",filter,false){ _running=true; } // get up a.s.a.p.
};

class H4P_SerialLogger: public H4P_EventListener{ 
    public:
        H4P_SerialLogger(uint32_t filter=H4PE_ALL): 
            H4P_EventListener(filter,[](const std::string& s,H4PE_TYPE t,const std::string& m){ Serial.printf("SLOG: %s %s %s\n",CSTR(s),CSTR(h4pGetEventName(t)),CSTR(m)); }){}
};