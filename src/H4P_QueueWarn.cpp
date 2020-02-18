/*
 MIT License

Copyright (c) 2019 Phil Bowles <h4plugins@gmail.com>
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
void H4P_QueueWarn::show(){
    reply("Qwarn: capacity=%d warn when size > %d\n",h4._capacity(),limit);
    reply("Qwarn: max used=%d [%d%%]\n",maxq,(maxq*100)/h4._capacity());
}

void H4P_QueueWarn::pcent(uint32_t pc){
    limit=constrain(__setLimit(pc),H4_Q_ABS_MIN,(uint32_t) h4._capacity());
    show();
}

uint32_t H4P_QueueWarn::_qwPcent(vector<string> vs){ return guardInt1(vs,bind(&H4P_QueueWarn::pcent,this,_1)); }
//
//      H4P_QueueWarn
//
H4P_QueueWarn::H4P_QueueWarn(function<void(bool)> _f,uint32_t _limit){
    _pid=qwrnTag();
    _hook=[this](){ run(); };
    _cmds={
        {_pid,     {H4PC_SHOW, 0, CMD(show)}},
        {_pid,     {H4PC_ROOT, subid, nullptr}},
        {"pcent",  {subid,   0, CMDVS(_qwPcent)}}
    };
    f=_f;  
    limit=__setLimit(_limit);
}

void H4P_QueueWarn::run(){ // optimise a la throttle
    static bool warned=false;
    uint32_t qsize=h4.size();
    if(qsize > maxq) maxq=qsize;
    if(qsize > limit) {
        if(!warned){
            f(true);
            warned=true;
        }
    }
    else {
        if(warned){
            f(false);
            warned=false;            
        }
    }
}