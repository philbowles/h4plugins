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
#include<H4P_LocalLogger.h>
#include <H4P_SerialCmd.h>

H4P_LocalLogger::H4P_LocalLogger(uint32_t limit,uint32_t filter):  _limit(limit), H4Plugin(H4PID_LLOG,filter | H4P_EVENT_FACTORY) {
    _fname="log.csv";
    _addLocals({
        {_pName,   {H4PC_H4, _pid, nullptr}},
        {"msg",    {_pid, 0, CMDNULL}},
        {"clear",  {_pid, 0, CMD(clear)}},
        {"flush",  {_pid, 0, CMD(flush)}}
    });
}

void H4P_LocalLogger::clear(){ HAL_FS.remove(CSTR(string("/"+_fname))); }

void H4P_LocalLogger::flush(){
    show();
    clear();
}

void H4P_LocalLogger::show(){
    uint32_t fsz=HAL_FS.open(CSTR(string("/"+_fname)),"r").size();
    reply("File /%s size=%u (%u %%full) limit=%u",CSTR(_fname),fsz,(fsz*100)/_limit,_limit);
    h4cmd._dump(vector<string>{_fname});
}
//
//      our raison d'etre
//
void H4P_LocalLogger::_handleEvent(H4PID pid,H4P_EVENT_TYPE t,const string& msg) {
    if(t==H4P_EVENT_FACTORY) clear();
    else {
        vector<string> msgparts={stringFromInt(millis()),h4pgetEventName(t),h4pnames[pid],msg};
        uint32_t size=h4cmd.write("/"+_fname,join(msgparts,",")+"\n","a");
        if(size > _limit) flush();
    }
}