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
#ifndef H4P_TaskSniffer_HO
#define H4P_TaskSniffer_HO

#include<H4PCommon.h>

class H4P_TaskSniffer: public H4Plugin{
    protected:
//
//      machinery
//
        uint32_t            __incexc(vector<string> vs,function<void(vector<uint32_t>)> f);

        void                _alwaysExclude();
//
//      cmd responders
//
        VSCMD(_tsInclude);
        VSCMD(_tsExclude);
//         
        unordered_set<uint32_t> hitList;

                void        _common();

                void        _taskDump(H4_TASK_PTR t,char c);

    public:
        H4P_TaskSniffer();
        H4P_TaskSniffer(uint32_t i);
        H4P_TaskSniffer(initializer_list<uint32_t> i);

                void        include(initializer_list<uint32_t> i){ hitList.insert(i);_alwaysExclude(); }
                void        include(uint32_t i){ include({i}); }
                void        include(vector<uint32_t> i){ for(auto const& l:i)include(l); }
                
                void        exclude(uint32_t i){ exclude({i}); }
                void        exclude(initializer_list<uint32_t> i){ for(auto const& l:i) hitList.erase(l); }
                void        exclude(vector<uint32_t> i){ for(auto const& l:i) hitList.erase(l); }
        
                void        show();
};

extern __attribute__((weak)) H4P_TaskSniffer h4ts;

#endif // H4P_TaskSniffer_HO