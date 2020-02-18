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
#include<H4P_TaskSniffer.h>

uint32_t H4P_TaskSniffer::__incexc(vector<string> vs,function<void(vector<uint32_t>)> f){
    return guard1(vs,[f,this](vector<string> vs){
        auto vi=expectInt(PAYLOAD);
        if(vi.size()) return ([f,this](vector<uint32_t> vu){ 
            f(vu);
            show(); 
            return H4_CMD_OK;
        })(vi);
        else {
            if((PAYLOAD).find_first_of("-")!=string::npos){
                vector<uint32_t> range=expectInt(PAYLOAD,"-");
                if(range.size()==2 && range[0]<range[1]){
                    range[1]=std::min(range[1],(uint32_t) 99);
                    vector<uint32_t> expanded;
                    for(uint32_t i=range[0];i<range[1]+1;i++) expanded.push_back(i);
                    f(expanded);
                    show();
                    return H4_CMD_OK;
                } else return H4_CMD_PAYLOAD_FORMAT;
            } else return H4_CMD_NOT_NUMERIC;
        }
    });    
}

void H4P_TaskSniffer::_alwaysExclude(){ // still need this?
    exclude({
 //       H4P_TRID_SQWV,
//        H4P_TRID_WIFI,
//        H4P_TRID_MQTT,
//        H4P_TRID_ASWS,
//        H4P_TRID_UBSW
    }); 
}        

void H4P_TaskSniffer::_common(){
    _pid=snifTag();
//    _names={ {H4P_TRID_SNIF,uppercase(_pid)} };
    _cmds={
        {_pid,      {H4PC_SHOW, 0, CMD(show)}},
        {_pid,      {H4PC_ROOT, subid, nullptr}},
        {"include", {subid, 0, CMDVS(_tsInclude)}},
        {"exclude", {subid, 0, CMDVS(_tsExclude)}}
    };
    h4._hookEvent(bind(&H4P_TaskSniffer::_taskDump,this,_1,_2));    
}

void H4P_TaskSniffer::_taskDump(H4_TASK_PTR t,const char c){
    if(hitList.count((t->uid)%100)) {
        reply("%d:%u:%c: ",h4.size(),micros(),c);
        h4._dumpTask(t);
    }
}
//
//      cmd responders
//
uint32_t H4P_TaskSniffer::_tsExclude(vector<string> vs){ return __incexc(vs,[this](vector<uint32_t> vi){ exclude(vi); }); }

uint32_t H4P_TaskSniffer::_tsInclude(vector<string> vs){ return __incexc(vs,[this](vector<uint32_t> vi){ include(vi); }); }

void H4P_TaskSniffer::show(){
    vector<uint32_t> sorted;
    reply("ts List: \n");
    for(auto const& h:hitList) sorted.push_back(h);
    sort(sorted.begin(),sorted.end());
    for(auto const& s:sorted) { reply("%d, ",s); }
    reply("\n");
}
//
//      public 
//
H4P_TaskSniffer::H4P_TaskSniffer(){ 
    for(uint32_t i=0;i<100;i++) hitList.insert(i);
    _alwaysExclude();
    _common();
}

H4P_TaskSniffer::H4P_TaskSniffer(uint32_t i){ 
    include(i);
    _common();
}

H4P_TaskSniffer::H4P_TaskSniffer(initializer_list<uint32_t> i){ 
    include(i);
    _common();
}
