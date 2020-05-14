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
#ifndef H4P_HeapWarn_HO
#define H4P_HeapWarn_HO

#include<H4PCommon.h>

class H4P_HeapWarn: public H4Plugin {
        VSCMD(_hwPcent);

        function<void(bool)>    _f;
        uint32_t                _initial;
        uint32_t                _limit;
        uint32_t                _minh=0;

            void        _run();
            uint32_t    _setLimit(uint32_t v);
            void        _start() override {
                h4._hookLoop([this](){ _run(); },_subCmd);
                H4Plugin::_start();
            }
            void        _stop() override {
                h4._unHook(_subCmd);
                H4Plugin::_stop();
            }
    public:
        H4P_HeapWarn(function<void(bool)> f,uint32_t pc=50);

        void        show() override;
        void        pcent(uint32_t pc);
};

#endif // H4P_HeapWarn_Hs