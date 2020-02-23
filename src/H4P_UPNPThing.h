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
#ifndef H4P_UPNPThing_HO
#define H4P_UPNPThing_HO

#include<H4PCommon.h>
#include<H4P_SerialCmd.h>
#include<H4P_WiFiSelect.h>
#include<H4P_WiFi.h>
#include<H4P_BinaryThing.h>
#include<H4P_AsyncWebServer.h>
#include<H4P_UPNPCommon.h>

#ifndef H4P_NO_WIFI


class H4P_UPNPThing: public H4P_BinaryThing, public H4P_UPNPCommon {
        void        _hookIn() override{ H4P_UPNPCommon::_pseudoHookIn(); }
        bool        _getState() override { return H4P_BinaryThing::_getState(); }
        void        _turn(bool b,const string& src) override { H4P_BinaryThing::_turn(b,src); }
    public:
        H4P_UPNPThing(const string& name,H4BS_FN_SWITCH f=[](bool){},bool initial=OFF):
            H4P_BinaryThing(f,initial),
            H4P_UPNPCommon(name){
        }
};

#endif
#endif // H4P_UPNPThing_H