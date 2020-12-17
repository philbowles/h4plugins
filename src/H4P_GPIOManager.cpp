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
#include<H4P_GPIOManager.h>
#include <numeric>

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
void H4GPIOPin::_pinFactoryCommon(bool onof){ // optimise for no logging
    if(onof){
        H4P_BinaryThing* btp=reinterpret_cast<H4P_BinaryThing*>(H4Plugin::isLoaded(onofTag()));
        if(btp){
            onEvent=[this,btp](H4GPIOPin* pp){ 
                Serial.printf("BTP onEvent pp -> %d\n",pp->logicalRead());
                if(pp->style==H4GM_PS_LATCHING) {
                    if(pp->nEvents) btp->toggle(); // POTENTIAL BUG! if btp already commanded on, 1st time wont toggle
    #ifdef H4P_LOG_EVENTS
                } else btp->_turn(pp->logicalRead(),gpioTag()+string("("+stringFromInt(pin)+"/"+stringFromInt(style)+")"));
    #else
                } else btp->turn(pp->logicalRead());
    #endif
            };
        } else { 
            Serial.println("FATAL: 'xSource' needs onof!");
            return;
        }
    }
    H4P_GPIOManager::pins[pin]=this;
    begin();
}

#ifdef H4P_LOG_EVENTS
string H4GPIOPin::dump(){
    char buf[128];
    sprintf(buf,"%2d %2d %2d %2d %6d %6d %6d %6d %6d %6d %6d %6d",pin,gpioType,style,sense,Tevt,state,delta,rate,Rpeak,cps,cMax,(int) nEvents);
    return(buf);
}
#endif

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
    if(cps < cMax){
        _throttle(false);
        if(digitalRead(pin)==(state ^ sense)){
            stampEvent();
            _toggleState();
        }
    } else _throttle(true);
}
//
//      H4P_GPIOManager
//
void H4P_GPIOManager::_start(){
    h4._hookLoop([this](){ _run(); },_subCmd);
    h4.every(1000,[this](){
        for(auto p:pins){
            H4GPIOPin* ptr=p.second;         
            if(!((ptr->cps) < ptr->cMax)) ptr->_syncState();
            ptr->rate=(ptr->cps);
            ptr->Rpeak=std::max(ptr->Rpeak,ptr->rate);
            ptr->cps=0;
        }
    },nullptr,H4P_TRID_GPIO,true);
    H4Plugin::_start();
}

void  H4P_GPIOManager::_run(){ for(auto const& p:pins) (p.second)->run(); }

H4P_GPIOManager::H4P_GPIOManager(): H4Plugin(gpioTag()){}
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
    latched=!state; // begin() calls sendState(), which toggles it, so "untoggle" it!
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
    Serial.printf("MultistagePin::sendEvent state=%d\n",state);
    if(state){
        stage=0;
        for(auto const& s:stageMap) {
            if(s.first) stageTimer.push_back(h4.once(s.first,[this](){
                held=stageMap[++stage].first;
                DebouncedPin::sendEvent();
                },nullptr,H4P_TRID_MULT)); 
        }
    }
    else {
        if(delta){
            for(auto const& t:stageTimer) h4.cancel(t);
            stageTimer.clear();stageTimer.shrink_to_fit(); // really?
            held=delta;
            stageMap[stage].second(this);
        } else DebouncedPin::sendEvent();// inital time only 
    }
}

PolledPin::PolledPin(uint8_t _p,uint8_t _g,H4GM_STYLE _s,uint8_t _a,uint32_t _f,uint32_t _v,H4GM_FN_EVENT _c): H4GPIOPin(_p,_g,_s,_a,_c),frequency(_f),isAnalog(_v){
    h4.every(frequency,[this](){ read(); },nullptr,H4P_TRID_POLL);
    if(isAnalog) state=analogRead(pin);
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

AnalogAveragePin::AnalogAveragePin(uint8_t _p,uint32_t _f,uint32_t n,H4GM_FN_EVENT _c): _n(n), PolledPin(_p,INPUT,H4GM_PS_THRESHA,ACTIVE_HIGH,_f,true,_c) {}
void AnalogAveragePin::read(){
    _samples.push_back(analogRead(pin));
//    Serial.printf("sample %u=%u\n",_samples.size(),_samples.back());
    if(_samples.size() == _n){
        state=std::accumulate( _samples.begin(), _samples.end(), 0) / _samples.size();
        _samples.clear();
        sendEvent();
    };
}

AnalogThresholdPin::AnalogThresholdPin(uint8_t _p,uint32_t _f,uint32_t _l,H4GM_COMPARE _cf,H4GM_FN_EVENT _c): limit(_l), fCompare(_cf), PolledPin(_p,INPUT,H4GM_PS_THRESHA,ACTIVE_HIGH,_f,true,_c) {
    state=fCompare(analogRead(pin),limit); // refakta
}
void AnalogThresholdPin::read(){
    uint32_t inst=fCompare(analogRead(pin),limit);
    if(inst!=state){
        stampEvent();
        _setState(inst);
    }
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
	if(state){
		if(timer) timer=h4.cancel(timer);
		else sendEvent();
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
/* do we need this ?
OutputPin* H4P_GPIOManager::isOutput(uint8_t p){
    H4GPIOPin* output;
    if((output=isManaged(p)) && output->style==H4GM_PS_OUTPUT) return reinterpret_cast<OutputPin*>(output);
    return nullptr;
}
*/

uint32_t H4P_GPIOManager::logicalRead(uint8_t p){ if(pins.count(p)) return pins[p]->logicalRead(); }

void H4P_GPIOManager::logicalWrite(uint8_t p,uint8_t l) { 
    if(isManaged(p)) if(pins[p]->style==H4GM_PS_OUTPUT) reinterpret_cast<OutputPin*>(pins[p])->logicalWrite(l);
 }

void H4P_GPIOManager::throttle(uint8_t p,uint32_t l,H4GM_FN_BOOL f) { if(isManaged(p)) pins[p]->throttle(l,f); }

void H4P_GPIOManager::toggle(uint8_t p){ if(isManaged(p))  reinterpret_cast<OutputPin*>(pins[p])->toggle();  }
//
//      (Oblique) Strategies
//
AnalogAveragePin* H4P_GPIOManager::AnalogAverage(uint8_t pin,uint32_t freq,uint32_t nSamples,H4GM_FN_EVENT callback){
    return pinFactory<AnalogAveragePin>(false,pin,freq,nSamples,callback);
}
AnalogThresholdPin* H4P_GPIOManager::AnalogThreshold(uint8_t pin,uint32_t freq,uint32_t threshold,H4GM_COMPARE compare,H4GM_FN_EVENT callback){
    return pinFactory<AnalogThresholdPin>(false,pin,freq,threshold,compare,callback);
}
AnalogThresholdPin* H4P_GPIOManager::AnalogThresholdSource(uint8_t pin,uint32_t freq,uint32_t threshold,H4GM_COMPARE compare){
    return pinFactory<AnalogThresholdPin>(true,pin,freq,threshold,compare,nullptr);
}

CircularPin* H4P_GPIOManager::Circular(uint8_t pin,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,uint32_t nStages,H4GM_FN_EVENT callback){
    return pinFactory<CircularPin>(false,pin,mode,H4GM_PS_CIRCULAR,sense,dbTimeMs,nStages,callback);
}

DebouncedPin* H4P_GPIOManager::Debounced(uint8_t pin,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4GM_FN_EVENT callback){
    return pinFactory<DebouncedPin>(false,pin, mode, H4GM_PS_DEBOUNCED, sense, dbTimeMs, callback);
}
DebouncedPin* H4P_GPIOManager::DebouncedSource(uint8_t pin,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs){
    return pinFactory<DebouncedPin>(true,pin, mode, H4GM_PS_DEBOUNCED, sense, dbTimeMs, nullptr);
}

EncoderPin* H4P_GPIOManager::Encoder(uint8_t pinA,uint8_t pinB,uint8_t mode,H4GM_SENSE sense,H4GM_FN_EVENT callback){
    EncoderPin* pa=pinFactory<EncoderPin>(false,pinA,mode, H4GM_PS_ENCODER_A, sense, nullptr);
    EncoderPin* pb=pinFactory<EncoderPin>(false,pinB,mode, H4GM_PS_ENCODER_B, sense, callback);
    return pb->_pairUp(pa);
} 
EncoderPin* H4P_GPIOManager::Encoder(uint8_t pinA,uint8_t pinB,uint8_t mode,H4GM_SENSE sense,int& i){
    return Encoder(pinA,pinB,mode,sense,[&i](H4GPIOPin* p){ i+=reinterpret_cast<EncoderPin*>(p)->encoderValue; });
}
EncoderPin* H4P_GPIOManager::EncoderSource(uint8_t pinA,uint8_t pinB,uint8_t mode,H4GM_SENSE sense){
    EncoderPin* pa=pinFactory<EncoderPin>(false,pinA,mode, H4GM_PS_ENCODER_A, sense, nullptr);
    EncoderPin* pb=pinFactory<EncoderPin>(true,pinB,mode, H4GM_PS_ENCODER_B, sense, nullptr);
    return pb->_pairUp(pa);
}

EncoderAutoPin* H4P_GPIOManager::EncoderAuto(uint8_t pinA,uint8_t pinB,uint8_t mode,H4GM_SENSE sense,int vMin,int vMax,int vSet,uint32_t vIncr,H4GM_FN_EVENT callback){
    EncoderAutoPin* pa=pinFactory<EncoderAutoPin>(false,pinA,mode, H4GM_PS_ENCODER_AUTO_A, sense, vMin, vMax, vIncr, nullptr);
    EncoderAutoPin* pb=pinFactory<EncoderAutoPin>(false,pinB,mode, H4GM_PS_ENCODER_AUTO_B, sense, vMin, vMax, vIncr, callback);
    if(vSet < vMin || vSet > vMax) pb->setCenter();
    else pb->setValue(vSet);    
    pb->_primary=false;
    pa->_otherPin=pb;
    pb->_otherPin=pa;
    return pb;
}

EncoderAutoPin* H4P_GPIOManager::EncoderAuto(uint8_t pinA,uint8_t pinB,uint8_t mode,H4GM_SENSE sense,int vMin,int vMax,int vSet,uint32_t vIncr,int& i){
    return EncoderAuto(pinA,pinB,mode,sense,vMin,vMax,vSet,vIncr,[&i](H4GPIOPin* p){ i=reinterpret_cast<EncoderAutoPin*>(p)->autoValue; });
}

FilteredPin* H4P_GPIOManager::Filtered(uint8_t pin,uint8_t mode,H4GM_SENSE sense,uint8_t filter,H4GM_FN_EVENT callback){
    return pinFactory<FilteredPin>(false,pin, mode, H4GM_PS_FILTERED, sense, filter, callback);
}

LatchingPin* H4P_GPIOManager::Latching(uint8_t pin,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4GM_FN_EVENT callback){
    return pinFactory<LatchingPin>(false,pin, mode, H4GM_PS_LATCHING, sense, dbTimeMs, callback);
}
LatchingPin* H4P_GPIOManager::LatchingSource(uint8_t pin,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs){
    return pinFactory<LatchingPin>(true,pin, mode, H4GM_PS_LATCHING, sense, dbTimeMs,nullptr); // replace with nullotr and if?
}

MultistagePin* H4P_GPIOManager::Multistage(uint8_t pin,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4GM_STAGE_MAP stageMap,H4GM_FN_EVENT callback){
    return pinFactory<MultistagePin>(false,pin,mode,H4GM_PS_MULTISTAGE,sense,dbTimeMs,stageMap,callback);
}

OutputPin* H4P_GPIOManager::Output(uint8_t pin,H4GM_SENSE sense,uint8_t initial,H4GM_FN_EVENT callback){
    return pinFactory<OutputPin>(false,pin, H4GM_PS_OUTPUT, sense, initial, callback);
}

PolledPin* H4P_GPIOManager::Polled(uint8_t pin,uint8_t mode,H4GM_SENSE sense,uint32_t frequency,uint32_t isAnalog,H4GM_FN_EVENT callback){ // fix order
    return pinFactory<PolledPin>(false,pin, mode, H4GM_PS_POLLED, sense, frequency, isAnalog, callback);
}
PolledPin* H4P_GPIOManager::PolledSource(uint8_t pin,uint8_t mode,H4GM_SENSE sense,uint32_t frequency,uint32_t isAnalog){ // fix order
    return pinFactory<PolledPin>(true,pin, mode, H4GM_PS_POLLED, sense, frequency, isAnalog, nullptr);
}

RawPin* H4P_GPIOManager::Raw(uint8_t pin,uint8_t mode,H4GM_SENSE sense,H4GM_FN_EVENT callback){
    return pinFactory<RawPin>(false,pin, mode, H4GM_PS_RAW, sense, callback);
}
RawPin* H4P_GPIOManager::RawSource(uint8_t pin,uint8_t mode,H4GM_SENSE sense){
    return pinFactory<RawPin>(true,pin, mode, H4GM_PS_RAW, sense, nullptr);
}

RepeatingPin* H4P_GPIOManager::Repeating(uint8_t pin,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,uint32_t frequency,H4GM_FN_EVENT callback){
    return pinFactory<RepeatingPin>(false,pin, mode, H4GM_PS_REPEATING, sense, dbTimeMs, frequency, callback);
}

RetriggeringPin* H4P_GPIOManager::Retriggering(uint8_t pin,uint8_t mode,H4GM_SENSE sense,uint32_t timeout,H4GM_FN_EVENT callback){
    return pinFactory<RetriggeringPin>(false,pin, mode, H4GM_PS_REPEATING, sense, timeout, callback);
}
RetriggeringPin* H4P_GPIOManager::RetriggeringSource(uint8_t pin,uint8_t mode,H4GM_SENSE sense,uint32_t timeout){
    return pinFactory<RetriggeringPin>(true,pin, mode, H4GM_PS_REPEATING, sense, timeout, nullptr);
}

SequencedPin* H4P_GPIOManager::Sequenced(uint8_t pin,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4GM_FN_EVENT callback){
    return pinFactory<SequencedPin>(false,pin, mode, H4GM_PS_SEQUENCED, sense, dbTimeMs, callback);
}

TimedPin* H4P_GPIOManager::Timed(uint8_t pin,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4GM_FN_EVENT callback){
    return pinFactory<TimedPin>(false,pin, mode, H4GM_PS_TIMED, sense, dbTimeMs, 0, callback);
}
