/*
 MIT License

Copyright (c) 2020 Phil Bowles <H48266@gmail.com>
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
#pragma once

#include<H4PCommon.h>

class H4P_WiFi;
class H4P_AsyncMQTT;

STAG(condition);

class H4P_BinaryThing: public H4Plugin{
            uint32_t                _timeout;
            unordered_set<string>   _slaves;
    protected:
            H4P_WiFi*               _pWiFi;
            H4P_AsyncMQTT*          _pMQTT;
            H4BS_FN_SWITCH  _f;

                uint32_t            _autoOff(vector<string> vs){ return _guardInt1(vs,[this](uint32_t t){ autoOff(t); }); }

        virtual void                _greenLight() override ;
                void                _publish(bool b);
                void                _setSlaves(bool b);
        virtual void                _setState(bool b);
                uint32_t            _slave(vector<string> vs);
                void                _start() override;
                uint32_t            _switch(vector<string> vs){ return _guardInt1(vs,[this](bool b){ turn(b); }); }
    public:
            bool            _state=false;
        H4P_BinaryThing(H4BS_FN_SWITCH f=nullptr,bool initial=OFF,uint32_t timer=0): _f(f),_state(initial),_timeout(timer), H4Plugin(H4PID_ONOF) {
            autoOff(timer);
            _addLocals({
                {autoTag(),{H4PC_H4, 0, CMDVS(_autoOff)}},
                {"on",     {H4PC_H4, 0, CMD(turnOn)}},
                {"off",    {H4PC_H4, 0, CMD(turnOff)}},
                {"state",  {H4PC_H4, 0, CMD(show)}},
                {"switch", {H4PC_H4, 0, CMDVS(_switch)}},
                {"toggle", {H4PC_H4, 0, CMD(toggle)}}
            });
        }

        virtual void show() override { 
                reply("State:%d, auto:%d",_state,_timeout);
                for(auto s:_slaves) reply("slave: %s",CSTR(s));
            }

                void        autoOff(uint32_t T){ 
                    _timeout=T;
                    _cb[autoTag()]=stringFromInt(T);
                }
                uint32_t    getAutoOff(){ return _timeout; }
                void        slave(const string& otherh4,bool inout=true){ 
                    if(inout) _slaves.insert(otherh4);
                    else _slaves.erase(otherh4);
                    }
                bool        state() { return _state; }
                void        turnOff(){ turn(false); }
                void        turnOn(){ turn(true); }
                void        toggle(){ turn(!_state); }
                void        turn(bool b);
#if H4P_LOG_EVENTS
                void        _turn(bool b,const string& src);
#endif
};
using H4_FN_CPRED      = function<uint32_t(bool)>;

class H4P_ConditionalThing: public H4P_BinaryThing{
                H4_FN_CPRED _predicate;
    protected:
                void        _greenLight() override;
                void        _setState(bool b) override;
    public:
        H4P_ConditionalThing(H4_FN_CPRED predicate,H4BS_FN_SWITCH f=nullptr,bool initial=OFF,uint32_t timer=0): 
            _predicate(predicate),
            H4P_BinaryThing(f,initial,timer) {}

                void        show() override { 
                    reply("Condition: %d",_predicate(_state));
                    H4P_BinaryThing::show();
                }
                void        syncCondition();
};