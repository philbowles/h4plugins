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
#pragma once

class h4proxy {
            h4proxy&    _set(const string& s);
    public:
            bool        _save;
            string      _id="";
            string      _v="";

        h4proxy(){};
        h4proxy(const string& name,const string& value,bool save=false): _id(name),_v(value),_save(save){}
        h4proxy& operator=(const string& s){ return _set(s); }
        h4proxy& operator=(const h4proxy& s);
        h4proxy& operator=(const int i){ return _set(stringFromInt(i)); }
        string& operator+(const string& s) const { static string tmp=_v + s; return tmp; }
        const char* c_str() const { return _v.c_str(); }
        operator const string& () const { return _v; }
        bool operator!=(const string& s) const { return s!=_v; }
        bool operator==(const string& s) const { return s==_v; }

        const string get() const { return _id+UNIT_SEPARATOR+_v; }
};

using H4P_PROXY_MAP =std::unordered_map<std::string,h4proxy>;