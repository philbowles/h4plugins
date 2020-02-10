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
#include<H4P_GPIOManager.h>

H4GPIOPin::H4GPIOPin(            
    uint8_t _p,
    uint8_t _g,
    H4GM_STYLE _s,
    uint8_t _a,
    H4GM_FN_EVENT _f
    ):            
    pin(_p),
    gpioType(_g),
    style(_s),
    sense(_a),
    onEvent(_f){
        pinMode(_p,_g);
       _syncState();
}
//
//  DIAGNOSTICS
//
void H4GPIOPin::dump(){ // tart this up
    Serial.print("PIN ");Serial.println(pin);
    Serial.print(" gpioType=");Serial.println(gpioType);
    Serial.print(" style=");Serial.println(style);
    Serial.print(" sense=");Serial.println(sense);
    Serial.print(" Tevt=");Serial.println(Tevt);
    Serial.print(" state=");Serial.println(state);
    Serial.print(" delta=");Serial.println(delta);
    Serial.print(" rate=");Serial.println(rate);
    Serial.print(" Rpeak=");Serial.println(Rpeak);
    Serial.print(" cps=");Serial.println(cps);
    Serial.print(" cMax=");Serial.println(cMax);
    Serial.print(" nEvents=");Serial.println(nEvents);
}
// end diag

void H4GPIOPin::stampEvent(){
    unsigned long now=micros();
    delta=now - Tevt;
    Tevt=now;      
    cps++;
}

void H4GPIOPin::_throttle(bool b){
    if(b ^ throttled) onThrottle(b);
    throttled=b;
}

void H4GPIOPin::run(){
    static uint32_t x=0;
//    if(!(x%30000)) Serial.printf("T=%u X pin=%d true=%d state=%d sense=%d norm=%d\n",millis(),pin,digitalRead(pin),state,sense,(state ^ sense));
    if(cps < cMax){
        _throttle(false);
        if(digitalRead(pin)==(state ^ sense)){
            stampEvent();
            _toggleState();
        }
    } else _throttle(true);
    x++;
}
//
//      H4P_GPIOManager
//
//#define SCALE 1 // cmd to change it

void  H4P_GPIOManager::run(){ for(auto const& p:pins) (p.second)->run(); }

void H4P_GPIOManager::_greenLight(){
    h4.every(1000,[this](){
        for(auto p:pins){
            H4GPIOPin* ptr=p.second;         
            if(!((ptr->cps) < ptr->cMax)) ptr->_syncState();
            ptr->rate=(ptr->cps);
            ptr->Rpeak=std::max(ptr->Rpeak,ptr->rate);
            ptr->cps=0;
        }
    },nullptr,H4P_TRID_GPIO,true);
}

H4P_GPIOManager::H4P_GPIOManager(){
    _pid=gpiotag();
    _hook=[this](){ run(); };
    _names={
        {H4P_TRID_GPIO,uppercase(_pid)},
        {H4P_TRID_DBNC,"DBNC"},
        {H4P_TRID_RPTP,"RPTP"},
        {H4P_TRID_POLL,"POLL"},
        {H4P_TRID_MULT,"MULT"},
        {H4P_TRID_TRIG,"TRIG"}
    };
    _cmds={ {"pins",    { H4PC_SHOW, 0, CMD(dump)}} };
}
//
//      (oblique) strategies
//
OutputPin::OutputPin(uint8_t _p,H4GM_STYLE _s,uint8_t _a,uint32_t _i,H4GM_FN_EVENT _c):H4GPIOPin(_p,OUTPUT,_s,_a,_c) {
    digitalWrite(pin,!(_i ^ sense)); // logicalWrite auto-sends event, we don't want that as begin() does it 
    state=_i;
}

void OutputPin::logicalWrite(uint8_t v) {
    if(state!=v){
        digitalWrite(pin,!(v ^ sense));
        state=v;
        sendEvent();
    } 
}

CircularPin::CircularPin(uint8_t _p,uint8_t _g,H4GM_STYLE _s,uint8_t _a,uint32_t _t,uint32_t _n,H4GM_FN_EVENT _c): SequencedPin(_p,_g,_s,_a,_t,_c),nStages(_n){}

void CircularPin::sendEvent() {
    if(stage == nStages) stage=0;
    SequencedPin::sendEvent();
}

DebouncedPin::DebouncedPin(uint8_t _p,uint8_t _g,H4GM_STYLE _s,uint8_t _a,uint32_t _t,H4GM_FN_EVENT _c): H4GPIOPin(_p,_g,_s,_a,_c),Td(_t) {}

void DebouncedPin::stateChange(){
	if(!bouncing){
		savedState=state;
		bouncing=h4.once(Td,
			[this](){
				bouncing=nullptr;
				if(state==savedState) sendEvent();
			},nullptr,H4P_TRID_DBNC);
	}
}

FilteredPin::FilteredPin(uint8_t _p,uint8_t _g,H4GM_STYLE _s,uint8_t _a,uint8_t _f,H4GM_FN_EVENT _c): H4GPIOPin(_p,_g,_s,_a,_c),filter(_f) {}

LatchingPin::LatchingPin(uint8_t _p,uint8_t _g,H4GM_STYLE _s,uint8_t _a,uint32_t _t,H4GM_FN_EVENT _c):CircularPin(_p,_g,_s,_a,_t,2,_c) {
    latched=!state; // begin() calls sendState(), which toggles it...so "untoggle" it!
}
void LatchingPin::lastCall(){
    latched=stage==1 ? ON:OFF;
    CircularPin::lastCall();
}

RawPin::RawPin(uint8_t _p,uint8_t _g,H4GM_STYLE _s,uint8_t _a,H4GM_FN_EVENT _c): H4GPIOPin(_p,_g,_s,_a,_c) {}

RepeatingPin::RepeatingPin(uint8_t _p,uint8_t _g,H4GM_STYLE _s,uint8_t _a,uint32_t _t,uint32_t _f,H4GM_FN_EVENT _c): DebouncedPin(_p,_g,_s,_a,_t,_c),frequency(_f){}
void RepeatingPin::sendEvent() {
    if(state) {
        sendTime(0);
        if(frequency) clock=h4.every(frequency,[this](){ sendTime(held+=(1000*frequency)); },nullptr,H4P_TRID_RPTP);
    }
    else {
        h4.cancel(clock);
        sendTime(delta);
    }
}

MultistagePin::MultistagePin(uint8_t _p,uint8_t _g,H4GM_STYLE _s,uint8_t _a,uint32_t _t,H4GM_STAGE_MAP _m,H4GM_FN_EVENT _c=[](H4GPIOPin*){}): DebouncedPin(_p,_g,_s,_a,_t,_c),stageMap(_m){}
void MultistagePin::sendEvent(){
    if(state){
        stage=0;
        for(auto const& s:stageMap) {
            if(s.first) stageTimer.push_back(h4.once(s.first,[this](){
                held=stageMap[++stage].first;
                H4GPIOPin::sendEvent();
                },nullptr,H4P_TRID_MULT)); 
        }
    }
    else {
        if(delta){
            for(auto const& t:stageTimer) h4.cancel(t);
            stageTimer.clear();stageTimer.shrink_to_fit();
            held=delta;
            stageMap[stage].second(this);
        } else H4GPIOPin::sendEvent();// inital time only 
    }
}

PolledPin::PolledPin(uint8_t _p,uint8_t _g,H4GM_STYLE _s,uint8_t _a,uint32_t _f,uint32_t _v,H4GM_FN_EVENT _c): H4GPIOPin(_p,_g,_s,_a,_c),frequency(_f),isAnalog(_v){
    h4.every(frequency,[this](){ read(); },nullptr,H4P_TRID_POLL);    
}
void PolledPin::read(){
    if(isAnalog){ // integrate with ::run? abstratc readAny ?
        uint32_t inst=analogRead(pin);
        if(inst!=state){
            stampEvent();
            _setState(inst);
        }
    } else H4GPIOPin::run();
}

SequencedPin::SequencedPin(uint8_t _p,uint8_t _g,H4GM_STYLE _s,uint8_t _a,uint32_t _t,H4GM_FN_EVENT _c): DebouncedPin(_p,_g,_s,_a,_t,_c){}
void SequencedPin::sendEvent() {
    if(state){
        ++stage;
          lastCall();
    }
}

RetriggeringPin::RetriggeringPin(uint8_t _p,uint8_t _g,H4GM_STYLE _s,uint8_t _a,uint32_t _t,H4GM_FN_EVENT _c): H4GPIOPin(_p,_g,_s,_a,_c),timeout(_t){}
void RetriggeringPin::stateChange(){
 //   Serial.print("SC ");Serial.println(state);
	if(state){
		if(timer) timer=h4.cancel(timer);
		else {
//            Serial.print("SE NO TIMER ");Serial.println(state);
            sendEvent();
        }
		timer=h4.once(timeout,[this]( ){
				timer=nullptr;
				sendEvent();
			}
		,nullptr,H4P_TRID_TRIG);
	} 
    else{
        if(!timer) sendEvent(); 
    }  
}

int8_t EncoderPin::rot_states[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0}; // valid(+-1) and invalid(0) rotation states
EncoderPin::EncoderPin(uint8_t _pA,uint8_t _g,H4GM_STYLE _s,uint8_t _a,H4GM_FN_EVENT _c): H4GPIOPin(_pA,_g,_s,_a,_c){}
void EncoderPin::stateChange(){
    if(_primary) _otherPin->stateChange();
    else {
        static int val=0;
        AB <<= 2;                  
        AB |= ((state<<1 | _otherPin->state) & 0x03); 
        val+=rot_states[AB & 0x0f];
        if(val==4 || val==-4){
            encoderValue=val/4;
            val=0;
            sendEvent();
        }
    }   
}

EncoderAutoPin::EncoderAutoPin(uint8_t _pA,uint8_t _g,H4GM_STYLE _s,uint8_t _a,int _vMin,int _vMax,uint32_t _vIncr,H4GM_FN_EVENT _c): 
    EncoderPin(_pA,_g,_s,_a,_c),vMin(_vMin),vMax(_vMax),vIncr(_vIncr){}

void EncoderAutoPin::setValue(int v){
    static int prev=0;
    autoValue=constrain(v,vMin,vMax);
    if(autoValue!=prev){
        prev=autoValue;
        EncoderPin::sendEvent();
    }
}		
//
//      H4P_GPIOManager
//
OutputPin* H4P_GPIOManager::isOutput(uint8_t p){
    H4GPIOPin* output;
    if((output=isManaged(p)) && output->style==H4GM_PS_OUTPUT) return reinterpret_cast<OutputPin*>(output);
    return nullptr;
}

uint32_t H4P_GPIOManager::logicalRead(uint8_t p){ if(pins.count(p)) return pins[p]->state; }

void H4P_GPIOManager::logicalWrite(uint8_t p,uint8_t l) { 
    if(isManaged(p)) if(pins[p]->style==H4GM_PS_OUTPUT) reinterpret_cast<OutputPin*>(pins[p])->logicalWrite(l);
 }

void H4P_GPIOManager::throttle(uint8_t p,uint32_t l,H4GM_FN_BOOL f) { if(isManaged(p)) pins[p]->throttle(l,f); }

void H4P_GPIOManager::toggle(uint8_t p){ if(isManaged(p))  reinterpret_cast<OutputPin*>(pins[p])->toggle();  }
//
//      (Oblique) Strategies
//
CircularPin* H4P_GPIOManager::Circular(uint8_t pin,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,uint32_t nStages,H4GM_FN_EVENT callback){
    return pinFactory<CircularPin>(pin,mode,H4GM_PS_CIRCULAR,sense,dbTimeMs,nStages,callback);
}

LatchingPin* H4P_GPIOManager::Latching(uint8_t pin,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4GM_FN_EVENT callback){
    return pinFactory<LatchingPin>(pin, mode, H4GM_PS_LATCHING, sense, dbTimeMs, callback);
}

DebouncedPin* H4P_GPIOManager::Debounced(uint8_t pin,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4GM_FN_EVENT callback){
    return pinFactory<DebouncedPin>(pin, mode, H4GM_PS_DEBOUNCED, sense, dbTimeMs, callback);
}

EncoderPin* H4P_GPIOManager::Encoder(uint8_t pinA,uint8_t pinB,uint8_t mode,H4GM_SENSE sense,H4GM_FN_EVENT callback){
    EncoderPin* pa=pinFactory<EncoderPin>(pinA,mode, H4GM_PS_ENCODER_A, sense, [](H4GPIOPin*){});
    EncoderPin* pb=pinFactory<EncoderPin>(pinB,mode, H4GM_PS_ENCODER_B, sense, callback);
    pb->_primary=false;
    pa->_otherPin=pb;
    pb->_otherPin=pa;
    return pb;
}
EncoderPin* H4P_GPIOManager::Encoder(uint8_t pinA,uint8_t pinB,uint8_t mode,H4GM_SENSE sense,int& i){
    return Encoder(pinA,pinB,mode,sense,[&i](H4GPIOPin* p){ i+=reinterpret_cast<EncoderPin*>(p)->encoderValue; });       
}

EncoderAutoPin* H4P_GPIOManager::EncoderAuto(uint8_t pinA,uint8_t pinB,uint8_t mode,H4GM_SENSE sense,int vMin,int vMax,int vSet,uint32_t vIncr,H4GM_FN_EVENT callback){
    EncoderAutoPin* pa=pinFactory<EncoderAutoPin>(pinA,mode, H4GM_PS_ENCODER_AUTO_A, sense, vMin, vMax, vIncr, [](H4GPIOPin*){});
    EncoderAutoPin* pb=pinFactory<EncoderAutoPin>(pinB,mode, H4GM_PS_ENCODER_AUTO_B, sense, vMin, vMax, vIncr, callback);
    pb->_primary=false;
    pa->_otherPin=pb;
    pb->_otherPin=pa;
    if(vSet < vMin || vSet > vMax) pb->setCenter();
    else pb->setValue(vSet);
    return pb;
}

EncoderAutoPin* H4P_GPIOManager::EncoderAuto(uint8_t pinA,uint8_t pinB,uint8_t mode,H4GM_SENSE sense,int vMin,int vMax,int vSet,uint32_t vIncr,int& i){
    return EncoderAuto(pinA,pinB,mode,sense,vMin,vMax,vSet,vIncr,[&i](H4GPIOPin* p){ i=reinterpret_cast<EncoderAutoPin*>(p)->autoValue; });
}

FilteredPin* H4P_GPIOManager::Filtered(uint8_t pin,uint8_t mode,H4GM_SENSE sense,uint8_t filter,H4GM_FN_EVENT callback){
    return pinFactory<FilteredPin>(pin, mode, H4GM_PS_FILTERED, sense, filter, callback);
}

MultistagePin* H4P_GPIOManager::Multistage(uint8_t pin,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4GM_STAGE_MAP stageMap,H4GM_FN_EVENT callback){
    return pinFactory<MultistagePin>(pin,mode,H4GM_PS_MULTISTAGE,sense,dbTimeMs,stageMap,callback);
}

OutputPin* H4P_GPIOManager::Output(uint8_t pin,H4GM_SENSE sense,uint8_t initial,H4GM_FN_EVENT callback){
    return pinFactory<OutputPin>(pin, H4GM_PS_OUTPUT, sense, initial, callback);
}

PolledPin* H4P_GPIOManager::Polled(uint8_t pin,uint8_t mode,H4GM_SENSE sense,uint32_t frequency,uint32_t isAnalog,H4GM_FN_EVENT callback){ // fix order
    return pinFactory<PolledPin>(pin, mode, H4GM_PS_POLLED, sense, frequency, isAnalog, callback);
}

RawPin* H4P_GPIOManager::Raw(uint8_t pin,uint8_t mode,H4GM_SENSE sense,H4GM_FN_EVENT callback){
    return pinFactory<RawPin>(pin, mode, H4GM_PS_RAW, sense, callback);
}

RepeatingPin* H4P_GPIOManager::Repeating(uint8_t pin,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,uint32_t frequency,H4GM_FN_EVENT callback){
    return pinFactory<RepeatingPin>(pin, mode, H4GM_PS_REPEATING, sense, dbTimeMs, frequency, callback);
}

RetriggeringPin* H4P_GPIOManager::Retriggering(uint8_t pin,uint8_t mode,H4GM_SENSE sense,uint32_t timeout,H4GM_FN_EVENT callback){
    return pinFactory<RetriggeringPin>(pin, mode, H4GM_PS_REPEATING, sense, timeout, callback);
}

SequencedPin* H4P_GPIOManager::Sequenced(uint8_t pin,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4GM_FN_EVENT callback){
    return pinFactory<SequencedPin>(pin, mode, H4GM_PS_SEQUENCED, sense, dbTimeMs, callback);
}

TimedPin* H4P_GPIOManager::Timed(uint8_t pin,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4GM_FN_EVENT callback){
    return pinFactory<TimedPin>(pin, mode, H4GM_PS_TIMED, sense, dbTimeMs, 0, callback);
}
//
//      DIAGNOSTICS
//

//
//      cmd functions
//
void H4P_GPIOManager::dump(){ for(auto const& p:pins) p.second->dump(); }
