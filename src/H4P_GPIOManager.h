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
#ifndef H4GPIOmanager_HO
#define H4GPIOmanager_HO

#include<H4PCommon.h>
#include<H4P_BinaryThing.h>
#include<climits>

#define H4GM_PASTE(x) x##Pin
#define H4GM_PIN(x) auto pin=reinterpret_cast<H4GM_PASTE(x) *>(ptr)

enum H4GM_STYLE:uint8_t {
    H4GM_PS_OUTPUT,
    H4GM_PS_CIRCULAR,
    H4GM_PS_DEBOUNCED,
    H4GM_PS_ENCODER_A,
    H4GM_PS_ENCODER_B,
    H4GM_PS_ENCODER_AUTO_A,
    H4GM_PS_ENCODER_AUTO_B,
    H4GM_PS_FILTERED,
    H4GM_PS_LATCHING,
    H4GM_PS_MULTISTAGE,
    H4GM_PS_POLLED,
    H4GM_PS_RAW,
    H4GM_PS_REPEATING,
    H4GM_PS_SEQUENCED,
    H4GM_PS_THRESHA,
    H4GM_PS_TIMED
};

enum H4GM_SENSE:uint8_t {
    ACTIVE_LOW,
    ACTIVE_HIGH
};

class  H4GPIOPin;

using H4GM_FN_BOOL   = function<void(bool)>;
using H4GM_PINMAP    = std::unordered_map<uint8_t,H4GPIOPin*>;
using H4GM_FN_EVENT  = function<void(H4GPIOPin*)>;
using H4GM_STAGE_MAP = vector<pair<uint32_t,H4GM_FN_EVENT>>;

class H4GPIOPin{
    friend class H4P_GPIOManager;
            bool            throttled=false;
            H4GM_FN_BOOL    onThrottle=[](bool){};
            void            _throttle(bool b);

    protected:
            void            _setState(uint32_t s){ // tidy
                                state=s;
                                stateChange();
                            }

            void            _syncState(){ state=!(digitalRead(pin) ^ sense); }

            void            _toggleState(){ _setState(!state); }

            void            throttle(uint32_t v,H4GM_FN_BOOL f){ 
                cMax=v;
                onThrottle=f;
            }

    virtual void            run();

            void            stampEvent();    

    virtual void            stateChange(){ sendEvent(); };

            void _pinFactoryCommon(bool onof);
    public:
            H4GM_FN_EVENT   onEvent=nullptr;
    virtual uint32_t        logicalRead(){ return state; }
            uint8_t         pin=0;                  // GPIO hardware pin number
            uint8_t         gpioType=0;             // INPUT, INOUT_PULLUP, OUTPUT etc
            H4GM_STYLE      style;                  // Strategy: Raw, debounced, retriggering etc
            uint8_t         sense=0;                // active HIGH or LOW
            unsigned long   Tevt=0;                 // uS time of last event
            int             state=0;                // 32 wide as it can hold analog value as well as digital 0/1
                                                    // and not uint because encoder returns -1 or +1
            uint32_t        delta=0;                // uS since last change
            uint32_t        rate=0;                 // instantaenous rate cps
            uint32_t        Rpeak=0;                // peak rate   
            uint32_t        cps=0;                  // sigma changes/sec
            uint32_t        cMax=UINT_MAX;          // max permitted cps
            uint32_t        nEvents=UINT_MAX;       // sigma "Events" (meaning depends on inheriting strategy)
            /*
                effectively set to -1. All pins "sendEvent" @ boot to allow user to register initial state
                sendEvent increments nEvents, so unless we do this hack, all pins will appear to have had
                an event, when in reality, they haven't 
            */

        virtual void        begin(){ lastCall(); }

        virtual void        lastCall(){ sendEvent(); }

        virtual void        sendEvent(){
                ++nEvents;
                if(onEvent) onEvent(this);
            }

            H4GPIOPin(uint8_t _p, uint8_t _g, H4GM_STYLE _s, uint8_t _a, H4GM_FN_EVENT _c);

        virtual ~H4GPIOPin(){}
#ifdef H4P_LOG_EVENTS
        virtual string        dump();
#endif
};
//
//  Inheritance order, not alphabetical
//
class RawPin: public H4GPIOPin {
    public:
        RawPin(uint8_t _p,uint8_t _g,H4GM_STYLE _s,uint8_t _a,H4GM_FN_EVENT _c);
        virtual ~RawPin(){}
};

class FilteredPin: public H4GPIOPin {
        uint32_t     filter;
    protected:
        void stateChange() override { if(state==filter) sendEvent(); }
    public:
#ifdef H4P_LOG_EVENTS
        virtual string dump () override {
            return H4GPIOPin::dump()
            .append(" F=").append(stringFromInt(filter)); 
        }
#endif
        FilteredPin(uint8_t _p,uint8_t _g,H4GM_STYLE _s,uint8_t _a,uint8_t _f,H4GM_FN_EVENT _c);
        virtual ~FilteredPin(){}
};

class DebouncedPin: public H4GPIOPin {
        uint32_t        Td;
        uint32_t        savedState;
        H4_TIMER        bouncing=nullptr;
    protected:
        virtual void    stateChange() override;
    public:
#ifdef H4P_LOG_EVENTS
        virtual string dump () override {
            return H4GPIOPin::dump()
            .append(" Tdb=").append(stringFromInt(Td)); 
        }
#endif
        DebouncedPin(uint8_t _p,uint8_t _g,H4GM_STYLE _s,uint8_t _a,uint32_t _d,H4GM_FN_EVENT _c);
        virtual ~DebouncedPin(){}
};

class PolledPin: public H4GPIOPin {
        uint32_t        frequency;
        bool            isAnalog;

                void            run() override {}
    protected:
        virtual void            read();
	public:
#ifdef H4P_LOG_EVENTS
        virtual string dump ()override {
            return H4GPIOPin::dump()
            .append(" f=").append(stringFromInt(frequency))
            .append(" A=").append(stringFromInt(isAnalog)); 
        }        
#endif
        PolledPin(uint8_t _p,uint8_t _g,H4GM_STYLE _s,uint8_t _a,uint32_t _f,uint32_t _v,H4GM_FN_EVENT _c);
        virtual ~PolledPin(){}
};

using H4GM_COMPARE=function<bool(uint32_t,uint32_t)>;
#define H4GM_LESS std::less<uint32_t>()
#define H4GM_GREATER std::greater<uint32_t>()

class AnalogAveragePin: public PolledPin {
        void        read() override;
        uint32_t            _n;
        vector<uint32_t>    _samples;
	public:
#ifdef H4P_LOG_EVENTS
        virtual string dump ()override {
            return PolledPin::dump()
            .append(" nSamples=").append(stringFromInt(_n)); 
        }        
#endif
        AnalogAveragePin(uint8_t _p,uint32_t f,uint32_t n,H4GM_FN_EVENT _c);
        virtual ~AnalogAveragePin(){}
};

class AnalogThresholdPin: public PolledPin {
        H4GM_COMPARE    fCompare;
        void        read() override;
	public:
        uint32_t    limit;
#ifdef H4P_LOG_EVENTS
        virtual string dump ()override {
            return PolledPin::dump()
            .append(" limit=").append(stringFromInt(limit)); 
        }        
#endif
        AnalogThresholdPin(uint8_t _p,uint32_t _f,uint32_t _l,H4GM_COMPARE _cf,H4GM_FN_EVENT _c);
        virtual ~AnalogThresholdPin(){}
};

class RetriggeringPin: public H4GPIOPin {
        H4_TIMER        timer=nullptr;
        uint32_t        timeout;
     protected:
        virtual void    stateChange() override;    
	public:
#ifdef H4P_LOG_EVENTS
        virtual string dump ()override {
            return H4GPIOPin::dump()            
            .append(" t/o=").append(stringFromInt(timeout)); 

        }        
#endif
        RetriggeringPin(uint8_t _p,uint8_t _g,H4GM_STYLE _s,uint8_t _a,uint32_t _t,H4GM_FN_EVENT _c);
        virtual ~RetriggeringPin(){}

        virtual void    begin() override { stateChange(); }   
};

class RepeatingPin: public DebouncedPin {
        uint32_t        frequency;
        H4_TIMER        clock;

    protected:
        virtual void    sendTime(uint32_t t){
                    held=t;
                    H4GPIOPin::sendEvent();
                }
                void    sendEvent() override;
	public:
        uint32_t    held=0;
#ifdef H4P_LOG_EVENTS
        virtual string dump ()override {
            return DebouncedPin::dump()
            .append(" f=").append(stringFromInt(frequency))
            .append(" held=").append(stringFromInt(held)); 
        }        
#endif
        RepeatingPin(uint8_t _p,uint8_t _g,H4GM_STYLE _s,uint8_t _a,uint32_t _d,uint32_t _f,H4GM_FN_EVENT _c);
        virtual ~RepeatingPin(){}
};
using TimedPin = RepeatingPin;

class MultistagePin: public DebouncedPin { // add dump
        vector<H4_TIMER>    stageTimer={};
        H4GM_STAGE_MAP      stageMap;  
        
        void    sendEvent() override;

	public:
        uint32_t        stage=0;
        uint32_t        held=0;
#ifdef H4P_LOG_EVENTS
        virtual string dump () override{
            string stages;
            for(auto const& s:stageMap) stages.append(",f:").append(stringFromInt(s.first)); 
            return DebouncedPin::dump()
            .append(" stg=").append(stringFromInt(stage)) 
            .append(" hld=").append(stringFromInt(held)).append(stages);
        }
#endif
        MultistagePin(uint8_t _p,uint8_t _g,H4GM_STYLE _s,uint8_t _a,uint32_t _d,H4GM_STAGE_MAP _m,H4GM_FN_EVENT _c);
        virtual ~MultistagePin(){}
};

class SequencedPin: public DebouncedPin {
    protected:
          virtual void      sendEvent() override;
          virtual void      lastCall(){ H4GPIOPin::sendEvent(); }      
	public:
#ifdef H4P_LOG_EVENTS
        virtual string dump () override {
            return DebouncedPin::dump()
            .append(" stage=").append(stringFromInt(stage));
        }        
#endif
        uint32_t    stage=0; // current stage of cycle
		SequencedPin(uint8_t _p,uint8_t _g,H4GM_STYLE _s,uint8_t _a,uint32_t _d,H4GM_FN_EVENT _c);
        virtual ~SequencedPin(){}
};

class CircularPin: public SequencedPin {
    protected:
        virtual void     sendEvent() override;          
	public:
#ifdef H4P_LOG_EVENTS
        virtual string dump () override{
            return SequencedPin::dump()
            .append(" N=").append(stringFromInt(nStages));
        }        
#endif
        uint32_t	nStages=0; // cycle length
		CircularPin(uint8_t _p,uint8_t _g,H4GM_STYLE _s,uint8_t _a,uint32_t _d,uint32_t nStages,H4GM_FN_EVENT _c);
        virtual ~CircularPin(){}
};

class LatchingPin: public CircularPin {
	protected:
        virtual void    lastCall() override;
    public:
        uint32_t    logicalRead() override { return latched; }
#ifdef H4P_LOG_EVENTS
        virtual string dump ()override{
            return CircularPin::dump()
            .append(" L=").append(stringFromInt(latched));
        }        
#endif
        uint32_t   	latched; // logical state
        LatchingPin(uint8_t _p,uint8_t _g,H4GM_STYLE _s,uint8_t _a,uint32_t _d,H4GM_FN_EVENT _c);
        virtual ~LatchingPin(){}
};

class OutputPin: public H4GPIOPin {
                void    run(){}
    public:
                void     logicalWrite(uint8_t);

                void     toggle(){ logicalWrite(!state); }
        OutputPin(uint8_t _p,H4GM_STYLE _s,uint8_t _a,uint32_t _i,H4GM_FN_EVENT _c);
        virtual ~OutputPin(){}
};

class EncoderPin: public H4GPIOPin{
	protected:
        
        static int8_t      rot_states[];//   {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0}; // make static
		       uint8_t     AB = 0x03;

		void            stateChange();
	public:
        EncoderPin* _pairUp(EncoderPin* pa){
            _primary=false;
            pa->_otherPin=this;
            _otherPin=pa;
            return this;
        }
        uint32_t    logicalRead() override { return constrain(encoderValue,0,1); } // constrain
#ifdef H4P_LOG_EVENTS
        virtual string dump ()override{
            return H4GPIOPin::dump()
            .append(" EV=").append(stringFromInt(encoderValue));
        }        
#endif
        bool            _primary=true;
		EncoderPin*     _otherPin=nullptr;

		int             encoderValue=0;

		EncoderPin(uint8_t _pA,uint8_t _g,H4GM_STYLE _s,uint8_t _a,H4GM_FN_EVENT _c);
        virtual ~EncoderPin(){}
};

class EncoderAutoPin: public EncoderPin{
	protected:
        virtual void    sendEvent() override { setValue(autoValue+(vIncr*encoderValue)); }
        int             vMin,vMax,vIncr;
	public:
        uint32_t    logicalRead() override { return autoValue; }
        int             autoValue=0;
#ifdef H4P_LOG_EVENTS
        virtual string dump ()override{
            return EncoderPin::dump()
            .append(" vMin=").append(stringFromInt(vMin))
            .append(" vMax=").append(stringFromInt(vMax))
            .append(" vI=").append(stringFromInt(vIncr))
            .append(" AV=").append(stringFromInt(autoValue));
        }        
#endif
		EncoderAutoPin(uint8_t _pA,uint8_t _g,H4GM_STYLE _s,uint8_t _a,int vMin, int vMax, uint32_t vIncr,H4GM_FN_EVENT _c);
        virtual ~EncoderAutoPin(){}

		void setValue(int v);
		void setMin(){ setValue(vMin);	}	
		void setMax(){ setValue(vMax);	}		
		void setPercent(uint32_t pc){ setValue((vMax-vMin)*pc/100); }		
		void setCenter(){ setValue((vMin+vMax)/2); }

        void begin() override {};
};
//
//      H4P_GPIOManager
//
class H4P_GPIOManager: public H4Plugin{//
        void                _start() override;
        void                _stop() override {
            h4._unHook(_subCmd);
            h4.cancelSingleton(H4P_TRID_GPIO);
            H4Plugin::_stop();
        }
        void                _run();
    public:
        template<typename T, typename... Args>
        T* pinFactory(bool onof,uint8_t _p,Args... args) {
            T*  pinclass=new T(_p,args...);
            pinclass->_pinFactoryCommon(onof);
            return pinclass;
        }

                H4GPIOPin*      isManaged(uint8_t p){ return pins.count(p) ? pins[p]:nullptr; }
        static  H4GM_PINMAP     pins;
        H4P_GPIOManager();
        //              returns 32 not 8 as it can also analogRead and state will hold analog value as well as digital 1/0
                uint32_t        logicalRead(uint8_t p);
                void            logicalWrite(uint8_t p,uint8_t l);
                void            throttle(uint8_t p,uint32_t v,H4GM_FN_BOOL f=[](bool){});
                void            toggle(uint8_t p);
//
//      Strategies
//
        AnalogAveragePin*   AnalogAverage(uint8_t p,uint32_t freq,uint32_t nSamples,H4GM_FN_EVENT callback);
        AnalogThresholdPin* AnalogThreshold(uint8_t p,uint32_t freq,uint32_t threshold,H4GM_COMPARE compare,H4GM_FN_EVENT callback);//
        AnalogThresholdPin* AnalogThresholdSource(uint8_t p,uint32_t freq,uint32_t threshold,H4GM_COMPARE compare);//
        CircularPin*        Circular(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,uint32_t nStages,H4GM_FN_EVENT callback);//
        DebouncedPin*       Debounced(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4GM_FN_EVENT callback);//
        DebouncedPin*       DebouncedSource(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs);//
        EncoderPin*         Encoder(uint8_t pA,uint8_t pB,uint8_t mode,H4GM_SENSE sense,H4GM_FN_EVENT);
        EncoderPin*         Encoder(uint8_t pA,uint8_t pB,uint8_t mode,H4GM_SENSE sense,int&);
        EncoderPin*         EncoderSource(uint8_t pA,uint8_t pB,uint8_t mode,H4GM_SENSE sense);
        EncoderAutoPin*     EncoderAuto(uint8_t pA,uint8_t pB,uint8_t mode,H4GM_SENSE sense,int vMin,int vMax,int vSet,uint32_t vIncr,H4GM_FN_EVENT callback);
        EncoderAutoPin*     EncoderAuto(uint8_t pA,uint8_t pB,uint8_t mode,H4GM_SENSE sense,int vMin,int vMax,int vSet,uint32_t vIncr,int&);
        FilteredPin*        Filtered(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint8_t filter,H4GM_FN_EVENT callback);//
        LatchingPin*        Latching(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4GM_FN_EVENT callback);//
        LatchingPin*        LatchingSource(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs);//
        MultistagePin*      Multistage(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4GM_STAGE_MAP stageMap,H4GM_FN_EVENT callback);//
        OutputPin*          Output(uint8_t p,H4GM_SENSE sense,uint8_t initial,H4GM_FN_EVENT callback=nullptr);// FIX ptr type
        PolledPin*          Polled(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t frequency,uint32_t isAnalog,H4GM_FN_EVENT callback);//
        PolledPin*          PolledSource(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t frequency,uint32_t isAnalog);//
        RawPin*             Raw(uint8_t p,uint8_t mode,H4GM_SENSE sense,H4GM_FN_EVENT callback);//
        RawPin*             RawSource(uint8_t p,uint8_t mode,H4GM_SENSE sense);//
        RepeatingPin*       Repeating(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,uint32_t frequency,H4GM_FN_EVENT callback);//
		RetriggeringPin*    Retriggering(uint8_t _p, uint8_t _mode,H4GM_SENSE sense,uint32_t timeout, H4GM_FN_EVENT _callback);
		RetriggeringPin*    RetriggeringSource(uint8_t _p, uint8_t _mode,H4GM_SENSE sense,uint32_t timeout);
        SequencedPin*       Sequenced(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4GM_FN_EVENT callback); //
        TimedPin*           Timed(uint8_t p,uint8_t mode,H4GM_SENSE sense,uint32_t dbTimeMs,H4GM_FN_EVENT callback); //
//
#ifdef H4P_LOG_EVENTS
        void                 show() override {
            reply(" P  G  T  S   Tevt  state  delta   rate  Rpeak    cps   cMax  nEvts EXTRA");
            for(auto const& p:pins) reply(CSTR(p.second->dump()));
        };
#endif
};

extern __attribute__((weak)) H4P_GPIOManager h4gm;

#endif // inc guard