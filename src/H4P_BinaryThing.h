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
#ifndef H4P_NO_WIFI
    #include<H4P_AsyncMQTT.h>
#endif

class H4P_BinaryThing: public H4Plugin{
        uint32_t         _timeout;
    protected:
        bool            _state=false;
        H4BS_FN_SWITCH  _f;
        
        #ifdef H4P_NO_WIFI
            void         _publish(bool b){}
        #else                
            virtual void _hookIn() override { if(H4Plugin::isLoaded(mqttTag())) h4mqtt.hookConnect([this](){ _publish(_getState()); }); }
                    void  _publish(bool b){ if(H4Plugin::isLoaded(mqttTag())) h4mqtt.publishDevice(stateTag(),b); }
        #endif

        virtual bool        _getState() { return _state; }
        virtual void        _setState(bool b) { _state=b; }

                uint32_t    _showState(vector<string> vs){ reply("State: %s\n",_getState() ? "ON":"OFF"); return H4_CMD_OK; }
                uint32_t    _switch(vector<string> vs){ return guardInt1(vs,bind(&H4P_BinaryThing::turn,this,_1)); }
                void        _greenLight() override { if(_f) _f(_state); }
    public:
        H4P_BinaryThing(H4BS_FN_SWITCH f=nullptr,bool initial=OFF,uint32_t timer=0): _f(f),_state(initial),_timeout(timer){
            _pid=onofTag();
            _cmds={
                {"on",     {H4PC_ROOT, 0, CMD(turnOn)}},
                {"off",    {H4PC_ROOT, 0, CMD(turnOff)}},
                {"state",  {H4PC_ROOT, 0, CMDVS(_showState)}},
                {"switch", {H4PC_ROOT, 0, CMDVS(_switch)}},
                {"toggle", {H4PC_ROOT, 0, CMD(toggle)}}
            };
            _names={{H4P_TRID_BTTO,"BTTO"}};
        }
        bool state(){ return _getState(); }
        void turnOff(){ turn(false); }
        void turnOn(){ turn(true); }
        void toggle(){ turn(!_state); }
        #ifdef H4P_LOG_EVENTS
        void turn(bool b){ _turn(b,userTag()); }
        // syscall only
        void _turn(bool b,const string& src){
            h4.cancelSingleton(H4P_TRID_BTTO);
            if(b && _timeout) h4.once(_timeout,[this](){ _turn(OFF,"btto"); },nullptr,H4P_TRID_BTTO,true);
            if(b!=_getState()){
                _setState(b);
                if(_f) _f(_state);
                _publish(_state);
                SYSEVENT(H4P_LOG_H4,src,onofTag(),"%s",(_state ? "ON":"OFF"));
            }
        }
        #else
//        void turn(bool b){ _turn(b,userTag()); }
        // syscall only
        void turn(bool b){
            h4.cancelSingleton(H4P_TRID_BTTO);
            if(b && _timeout) h4.once(_timeout,[this](){ turn(OFF); },nullptr,H4P_TRID_BTTO,true);
            if(b!=_getState()){
                _setState(b);
                if(_f) _f(_state);
                _publish(_state);
            }
        }
        #endif
};

#endif // H4P_BinaryThing_H