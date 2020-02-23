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
#ifndef ARDUINO_ARCH_STM32
#include<H4P_LocalLogger.h>
//
H4P_LocalLogger::H4P_LocalLogger(uint32_t limit): H4PLogService(logTag()), _limit(limit) {
    _local={
        {_pid,     {H4PC_SHOW, 0, CMD(show)}},
        {"clear",  {subid, 0, CMD(clear)}},
        {"flush",  {subid, 0, CMD(flush)}}
    };
}

void H4P_LocalLogger::clear(){ SPIFFS.remove(CSTR(string("/").append(logTag()))); }

void H4P_LocalLogger::flush(){
    show();
    clear();
}

void H4P_LocalLogger::show(){ h4sc._dump(vector<string>{logTag()}); }
//
//      our raison d'etre
//
void H4P_LocalLogger::_logEvent(const string &msg,H4P_LOG_TYPE type,const string& source,const string& target){
    vector<string> msgparts={stringFromInt(millis()),stringFromInt(type),source,target,msg};
    uint32_t size=h4sc.write("/log",join(msgparts,",").append("\n"),"a");
    if(size > _limit) flush();
}
#endif