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
#include<H4P_LinkMaster.h>

void H4P_LinkMaster::_handleEvent(const std::string& svc,H4PE_TYPE t,const std::string& msg){
    if(_running && svc==stateTag()) for(auto s:_slaves) _pMQTT->xPublish(CSTR((s+"/h4/switch")),msg);
}

uint32_t H4P_LinkMaster::_slave(std::vector<std::string> vs){
    if(vs.size()<1) return H4_CMD_TOO_FEW_PARAMS;
    if(vs.size()>1) return H4_CMD_TOO_MANY_PARAMS;
    std::vector<std::string> parts=split(vs[0],",");
    if(parts.size()!=2) return H4_CMD_PAYLOAD_FORMAT;
    if(!stringIsNumeric(parts[1])) return H4_CMD_NOT_NUMERIC;
    uint32_t action=STOI(parts[1]);
    if(action > 1) return H4_CMD_OUT_OF_BOUNDS;
    if(_slaves.count(parts[0]) ^ action){
        slave(parts[0],action);
#if H4P_LOG_MESSAGES
        info();
#endif
        return H4_CMD_OK;
    }
    return H4_CMD_NOT_NOW;
}

#if H4P_LOG_MESSAGES
void H4P_LinkMaster::info(){ 
    H4Service::info(); 
    if(_slaves.size()){
        reply(" Slaves:");
        for(auto s:_slaves) reply("  %s",CSTR(s)); };
    }
#endif

void H4P_LinkMaster::slave(const std::string& otherh4,bool inout){ 
    SYSINFO("slave %s %s",CSTR(otherh4),inout ? "Added":"Removed");
    if(inout) _slaves.insert(otherh4);
    else _slaves.erase(otherh4);
}