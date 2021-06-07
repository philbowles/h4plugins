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
#include<H4P_QueueWarn.h>

uint32_t H4P_QueueWarn::__setLimit(uint32_t v){ return (h4._capacity()*v)/100; }
//
//      cmd responders
//
#if H4P_LOG_MESSAGES
void H4P_QueueWarn::info(){
    reply("Qwarn: capacity=%d warn when size > %d",h4._capacity(),limit);
    reply("Qwarn: max used=%d [%d%%]",maxq,(maxq*100)/h4._capacity());
}
#endif

void H4P_QueueWarn::pcent(uint32_t pc){
    limit=constrain(__setLimit(pc),H4_Q_ABS_MIN,(uint32_t) h4._capacity());
#if H4P_LOG_MESSAGES
    info();
#endif
}

uint32_t H4P_QueueWarn::_qwPcent(std::vector<std::string> vs){ return _guardInt1(vs,[this](uint32_t && i){ pcent(i); }); }
//
H4P_QueueWarn::H4P_QueueWarn(uint32_t _limit): H4Service("qwrn"){
    _addLocals({
        {_me,           {H4PC_H4, _pid, nullptr}},
        {pcentTag(),    {_pid,   0, CMDVS(_qwPcent)}}
    });
    limit=__setLimit(_limit);
}

void H4P_QueueWarn::_run(){
    static bool warned=false;
    uint32_t qsize=h4.size();
    if(qsize > maxq) maxq=qsize;
    bool state=qsize > limit;
    if(state ^ warned) SYSWARN("Q,%d,%d",state,qsize);
    warned=state;
}