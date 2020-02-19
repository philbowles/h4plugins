/*
 MIT License

Copyright (c) 2019 Phil Bowles <H48266@gmail.com>
   github     https://github.com/philbowles/H4
   blog       https://8266iot.blogspot.com
   groups     https://www.facebook.com/groups/esp8266questions/
              https://www.facebook.com/H4-Esp8266-Firmware-Support-2338535503093896/


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
#ifndef H4P_PersistentStorage_HO
#define H4P_PersistentStorage_HO

#ifndef ARDUINO_ARCH_STM32
#include<H4PCommon.h>

class H4P_PersistentStorage: public H4Plugin {
        H4P_CONFIG_BLOCK psRam={};
        H4P_FN_PSCHANGE    _f;

        VSCMD(_get);
        VSCMD(_set);
        
        void         _hookIn() override;
        void         _showItem(const string& n){ reply("%s=%s\n",CSTR(n),CSTR(psRam[n])); }
    public:
        string& operator[](const string& name){
            return psRam[name];
        }

        H4P_PersistentStorage(H4P_FN_PSCHANGE f=nullptr);

        void          clear();
        void          dec(const string& name){ if(isNumeric(psRam[name])) setint(name,getint(name)-1); }
        bool          exists(const string& name){ return psRam.count(name); }
        string        getstring(const string& name){ return psRam[name]; }
        int           getint(const string& name){ return atoi(CSTR(psRam[name])); }
        void          inc(const string& name){ if(isNumeric(psRam[name])) setint(name,getint(name)+1); }
        void          setstring(const string& name,const string& value);
        void          setint(const string& name,int value){ setstring(name,stringFromInt(value)); }
        void          showStore(){ for(auto const& p:psRam) _showItem(p.first); }
};

extern __attribute__((weak)) H4P_PersistentStorage h4ps;

#endif
#endif // H4P_PersistentStorage_H