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
#include<H4.h>
#if H4_HOOK_TASKS

#include<H4P_TaskSniffer.h>
#include<H4P_SerialCmd.h>

uint32_t H4P_TaskSniffer::__incexc(std::vector<std::string> vs,std::function<void(std::vector<uint32_t>)> f){
    return _guard1(vs,[f,this](std::vector<std::string> vs){
        auto vi=_expectInt(H4PAYLOAD);
        if(vi.size()) return ([f,this](std::vector<uint32_t> vu){ 
            f(vu);
//            info(); 
            return H4_CMD_OK;
        })(vi);
        else {
            if((H4PAYLOAD).find_first_of("-")!=std::string::npos){
                std::vector<uint32_t> range=_expectInt(H4PAYLOAD,"-");
                if(range.size()==2 && range[0]<range[1]){
                    range[1]=std::min(range[1],(uint32_t) 99);
                    std::vector<uint32_t> expanded;
                    for(uint32_t i=range[0];i<range[1]+1;i++) expanded.push_back(i);
                    f(expanded);
//                    info();
                    return H4_CMD_OK;
                } else return H4_CMD_PAYLOAD_FORMAT;
            } else return H4_CMD_NOT_NUMERIC;
        }
    });    
}
extern H4_INT_MAP h4TaskNames;
void H4P_TaskSniffer::_common(){
    _addLocals({
        {_me,       {H4PC_H4, _pid, nullptr}},
        {"include", {_pid, 0, CMDVS(_tsInclude)}},
        {"exclude", {_pid, 0, CMDVS(_tsExclude)}}
    });
    h4._hookTask([=](H4_TASK_PTR t,uint32_t faze){
        //reply("TS HOOK TASK 0x%08x F=%d nC=%d",t,faze,h4TaskNames.size());
        if(hitList.count((t->uid)%100)) reply("%s",H4::dumpTask(t,faze).data());
        });
}
//
uint32_t H4P_TaskSniffer::_tsExclude(std::vector<std::string> vs){ return __incexc(vs,[this](std::vector<uint32_t> vi){ exclude(vi); }); }

uint32_t H4P_TaskSniffer::_tsInclude(std::vector<std::string> vs){ return __incexc(vs,[this](std::vector<uint32_t> vi){ include(vi); }); }

#if H4P_LOG_MESSAGES
void H4P_TaskSniffer::info(){
    std::vector<uint32_t> sorted;
    reply("ts List: \n");
    for(auto const& h:hitList) sorted.push_back(h);
    sort(sorted.begin(),sorted.end());
    for(auto const& s:sorted) { reply("%d, ",s); }
    reply("\n");
}
#endif
//
//      public 
//
H4P_TaskSniffer::H4P_TaskSniffer(): H4Service(snifTag()){ 
    for(uint32_t i=0;i<100;i++) hitList.insert(i);
    _common();
}

H4P_TaskSniffer::H4P_TaskSniffer(uint32_t i): H4Service(snifTag()){ 
    include(i);
    _common();
}

H4P_TaskSniffer::H4P_TaskSniffer(std::initializer_list<uint32_t> i): H4Service(snifTag()){ 
    include(i);
    _common();
}
#endif