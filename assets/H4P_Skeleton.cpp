
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
#include<H4P_Skeleton.h>

H4P_Skeleton::H4P_Skeleton(const string& name,H4_FN_VOID onStart,H4_FN_VOID onStop): H4Plugin(name,onStart,onStop){
    _rebootHook=[this](){ reply("FREE SOME RESOURCES\n"); };
    _factoryHook=[this](){ reply("CLEAN UP SOME SHIT\n"); };
    _cmds={
        {_pName,       { H4PC_H4, _subCmd, nullptr}}, // root for this plugin
        {"rattle",     { _subCmd,         0, CMD(rattle)}}, // h4/ME/rattle
        {"bones",      { _subCmd,         0, CMDVS(_bones)}} // h4/ME/bones[PL]
    }; 
}

uint32_t H4P_Skeleton::_bones(vector<string> vs){
     Serial.print(CSTR(_pName));Serial.print(" _bones PAYLOAD=");Serial.println(CSTR(H4PAYLOAD));
     return H4_CMD_OK;
}

void H4P_Skeleton::_hookIn(){
// hook to any service you depend on
    Serial.print(CSTR(_pName));Serial.println(" _hookIn"); 
}

void H4P_Skeleton::_greenLight(){
    Serial.print(CSTR(_pName));Serial.println(" _greenLight");
    start();
}

void H4P_Skeleton::_run(){
// once per loop - be very very careful
}

void H4P_Skeleton::rattle(){ Serial.print(CSTR(_pName));Serial.println(" rattle!"); }

void H4P_Skeleton::start(){
     Serial.print(CSTR(_pName));Serial.println(" _start"); 
}

void H4P_Skeleton::stop(){
    Serial.print(CSTR(_pName));Serial.println(" _stop"); 
}
