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

#if H4_HOOK_TASKS
class H4P_TaskSniffer: public H4Service{
        uint32_t            __incexc(std::vector<std::string> vs,std::function<void(std::vector<uint32_t>)> f);

        VSCMD(_tsInclude);
        VSCMD(_tsExclude);
//         
        std::unordered_set<uint32_t> hitList;

                void        _common();
    public:
        H4P_TaskSniffer();
        H4P_TaskSniffer(uint32_t i);
        H4P_TaskSniffer(std::initializer_list<uint32_t> i);

                void        include(std::initializer_list<uint32_t> i){ hitList.insert(i); }
                void        include(uint32_t i){ include({i}); }
                void        include(std::vector<uint32_t> i){ for(auto const& l:i)include(l); }
                
                void        exclude(uint32_t i){ exclude({i}); }
                void        exclude(std::initializer_list<uint32_t> i){ for(auto const& l:i) hitList.erase(l); }
                void        exclude(std::vector<uint32_t> i){ for(auto const& l:i) hitList.erase(l); }
#if H4P_LOG_MESSAGES
                void        info() override;
#endif
};
#else
struct H4P_TaskSniffer{};
#endif