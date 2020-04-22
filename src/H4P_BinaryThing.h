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
#ifndef H4P_BinaryThing_HO
#define H4P_BinaryThing_HO

#include<H4PCommon.h>
#include<H4P_SerialCmd.h>
#include<H4P_WiFiSelect.h>

class H4P_BinaryThing: public H4Plugin{
            uint32_t        _timeout;
            unordered_set<string>   _slaves;
    protected:
            bool            _state=false;
            H4BS_FN_SWITCH  _f;

                    bool     _getState() { return _state; }
            virtual void     _hookIn() override ;
                    void     _publish(bool b);
                    void     _setSlaves(bool b);
            virtual void     _setState(bool b);               
                    uint32_t _slave(vector<string> vs); //vscmd               
                    void     _start() override;
                    uint32_t _switch(vector<string> vs){ return guardInt1(vs,bind(&H4P_BinaryThing::turn,this,_1)); }
    public:
        H4P_BinaryThing(H4BS_FN_SWITCH f=nullptr,bool initial=OFF,uint32_t timer=0): _f(f),_state(initial),_timeout(timer), H4Plugin(onofTag()) {
            _cmds={
                {"on",     {H4PC_H4, 0, CMD(turnOn)}},
                {"off",    {H4PC_H4, 0, CMD(turnOff)}},
                {"state",  {H4PC_H4, 0, CMD(show)}},
                {"switch", {H4PC_H4, 0, CMDVS(_switch)}},
                {"toggle", {H4PC_H4, 0, CMD(toggle)}}
            };
        }

            void show() override { 
                reply("State: %s",_getState() ? "ON":"OFF");
                for(auto s :_slaves) reply("Slave: %s",CSTR(s));
            }

                    void    slave(const string& otherh4){ _slaves.insert(otherh4); }
                    bool    state() { return _getState(); }
                    void    turnOff(){ turn(false); }
                    void    turnOn(){ turn(true); }
                    void    toggle(){ turn(!_state); }
                    void    turn(bool b);
#ifdef H4P_LOG_EVENTS
                    void    _turn(bool b,const string& src);
#endif
};

#endif // H4P_BinaryThing_H