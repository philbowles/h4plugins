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
#include<H4P_PersistentStorage.h>
#include<H4P_SerialCmd.h>

#define SEPARATOR "\xff"

void H4P_PersistentStorage::_hookIn() {

    if(H4Plugin::isLoaded(scmdTag())){
        vector<string> items=split(H4P_SerialCmd::read("/"+_pid),SEPARATOR);
        for(auto const& i:items){
            vector<string> nv=split(i,"=");
            psRam[nv[0]]=nv[1];
        }
        H4PluginService::hookFactory([this](){ clear(); });
    } else { DEPENDFAIL(scmd); }
}

H4P_PersistentStorage::H4P_PersistentStorage(H4P_FN_PSCHANGE f): _f(f){
    _pid=storTag();
    _cmds={
        {_pid,      { H4PC_SHOW, 0, CMD(showStore) }},
        {_pid,      { H4PC_ROOT, subid, nullptr}},
        {"clear",   { subid, 0, CMD(clear)}},
        {"get",     { subid, 0, CMDVS(_get)}},
        {"set",     { subid, 0, CMDVS(_set)}}
    };
}

void  H4P_PersistentStorage::setstring(const string& name,const string& value){
    if(psRam[name]!=value){
        psRam[name]=value;
        //
        string items;
        for(auto const& p:psRam) items+=p.first+"="+p.second+SEPARATOR;
        items.pop_back();
        H4P_SerialCmd::write("/"+_pid,items);
        //
        if(_f) _f(name,value);
    }
}

uint32_t H4P_PersistentStorage::_get(vector<string> vs){
    return H4Plugin::guard1(vs,[this](vector<string> vs){
        if(psRam.count(PAYLOAD)) _showItem(PAYLOAD);
        return H4_CMD_OK;
    });
}

uint32_t H4P_PersistentStorage::_set(vector<string> vs){
    return guardString2(vs,[this](string a,string b){ 
        setstring(a,b);
        _showItem(a); //
    }); 
}

void     H4P_PersistentStorage::clear(){
    psRam.clear();
    SPIFFS.remove(CSTR(string("/"+_pid)));
}
#endif