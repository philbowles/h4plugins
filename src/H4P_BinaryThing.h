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

#include<H4Service.h>
#include<H4P_SerialCmd.h>
#include<H4P_WiFi.h>

STAG(condition);

extern bool h4punlocked;

class H4P_BinaryThing: public H4Service{
                H4P_FN_VB           _thing;
                void                _onChange(bool b);
    protected:
                uint32_t            _autoOff(std::vector<std::string> vs){ return _guardInt1(vs,[this](uint32_t t){ autoOff(t); }); }
        virtual void                _handleEvent(const std::string& svc,H4PE_TYPE t,const std::string& msg) override;
                uint32_t            _switch(std::vector<std::string> vs){ return _guardInt1(vs,[this](bool b){ turn(b); }); }

    public:
        H4P_BinaryThing(H4P_FN_VB thingFunction,bool initial=OFF,uint32_t timer=0): _thing(thingFunction),H4Service(onofTag(),H4PE_GVCHANGE|H4PE_VIEWERS) {
            h4p.gvSetInt(stateTag(),initial,false);
            h4p.gvSetInt(autoOffTag(),timer,true);
            _addLocals({
                {"auto",    {H4PC_H4, 0, CMDVS(_autoOff)}},
                {"on",      {H4PC_H4, 0, CMD(turnOn)}},
                {"off",     {H4PC_H4, 0, CMD(turnOff)}},
#if H4P_LOG_MESSAGES
                {stateTag(),{H4PC_H4, 0, CMD(info)}},
#endif
                {"switch",  {H4PC_H4, 0, CMDVS(_switch)}},
                {"toggle",  {H4PC_H4, 0, CMD(toggle)}}
            });
        }

#if H4P_LOG_MESSAGES
        virtual void info() override {
                    H4Service::info();
                    reply(" State:%d, AutoOff:%d",h4p.gvGetInt(stateTag()),h4p.gvGetInt(autoOffTag()));
                }   
#endif
                void        autoOff(uint32_t T);
                uint32_t    getAutoOff(){ return h4p.gvGetInt(autoOffTag()); }
                bool        state() { return h4p.gvGetInt(stateTag()); }
                void        turnOff(){ turn(OFF); }
                void        turnOn(){ turn(ON); }
                void        toggle(){ turn(!h4p.gvGetInt(stateTag())); }
                void        turn(bool b){ if(h4punlocked) h4p.gvSetInt(stateTag(),b); }
//
        virtual void        svcDown() override;
};

using H4_FN_CPRED      = std::function<bool()>;

class H4P_ConditionalThing: public H4P_BinaryThing{
                H4_FN_CPRED _predicate;
    protected:
        virtual void        _handleEvent(const std::string& svc,H4PE_TYPE t,const std::string& msg) override;
    public:
        H4P_ConditionalThing(H4P_FN_VB thingFunction,H4_FN_CPRED predicate,bool initial=OFF,uint32_t timer=0): 
            _predicate(predicate),
            H4P_BinaryThing(thingFunction,initial,timer) { syncCondition(); }
#if H4P_LOG_MESSAGES
                void        info() override { 
                    H4P_BinaryThing::info();
                    reply("Condition: %d",_predicate());
                }
#endif
                void        syncCondition();
};