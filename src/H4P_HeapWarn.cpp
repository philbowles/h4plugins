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
#include<H4P_HeapWarn.h>

H4P_HeapWarn::H4P_HeapWarn(uint32_t pc): H4Service("hwrn"){
    _addLocals({
        {_me,           {H4PC_H4, _pid, nullptr}},
        {pcentTag(),    {_pid,   0, CMDVS(_hwPcent)}}
    });
    _minh=_initial=_HAL_freeHeap();
    _limit=_setLimit(pc);
}

void H4P_HeapWarn::_run(){
    static bool warned=false;
    uint32_t hsize=_HAL_freeHeap();
    if(hsize < _minh) _minh=hsize;
    bool state=hsize < _limit;
    if(state ^ warned) SYSWARN("Heap,%d,%d",state,hsize);
    warned=state;
}

uint32_t H4P_HeapWarn::_hwPcent(std::vector<std::string> vs){ return _guardInt1(vs,[this](uint32_t && i){ pcent(i); }); }

uint32_t H4P_HeapWarn::_setLimit(uint32_t v){ return (_initial*v)/100; }
#define H4P_ABSMIN_HPCNT    5
void H4P_HeapWarn::pcent(uint32_t pc){
    _limit=constrain(_setLimit(pc),H4P_ABSMIN_HPCNT,(uint32_t) _initial);
#if H4P_LOG_MESSAGES
    info();
#endif
}

#if H4P_LOG_MESSAGES
void H4P_HeapWarn::info(){
    H4Service::info();
    reply(" Startvalue=%d warn when size < %d",_initial,_limit);
    reply(" Min level=%d [%d%%]",_minh,(_minh*100)/_initial);
}
#endif