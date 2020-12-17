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
#ifndef H4P_udpLogger_HO
#define H4P_udpLogger_HO

#include<H4P_WiFiSelect.h>
#include<H4PCommon.h>

class H4P_udpLogger: public H4Plugin {
        AsyncUDP 	    _udp;
        IPAddress		_ubIP;

            uint32_t            startheap,lastheap,delta,maxrate,limit;
            uint32_t            rps=0;

                void            _listenUDP();

                void            _hookIn() override;
                void            _start() override;
                void            _greenLight() override {}; // dont autostart!

    public: 
        /*
        D is smallest heap delta for which log message will be produced
        H is percentage of heap above which no reporting takes place at all

        In English: Log when heap below H% of start value and most recent change is > D bytes
        */               
        H4P_udpLogger(uint32_t D=32, uint32_t H=70): delta(D),H4Plugin("ulog"){
            _ubIP=IPAddress(239,255,255,250);
            lastheap=startheap=ESP.getFreeHeap();
            limit=(H*startheap)/100;
        }
};

#endif // H4P_udpLogger_H