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
//=========================================================================================================
class npANALOGTMP36: public npNODE { // aritmetic mean
    public:
        msg operator()(msg m) override {
            uint32_t mV=(m.load * 3275)/1024;
            m.load=mV - 500; // e only deal with ints, caller will haveto scale or truncate 
            return m;
        }
};
class npAVERAGE: public npNODE { // aritmetic mean
        int             _sigma=0;
        uint32_t        _reading=0;
        uint32_t        _n;
    public:
        npAVERAGE(uint32_t n): _n(n){}

        msg operator()(msg m) override {
            m.halt=true;
//            if(m.timer){
                if((_reading++) < (_n-1)) _sigma+=m.load;
                else {
                    m.load=_sigma / _n;
                    _sigma=_reading=0;
                    m.halt=false;
                }
//            }
            return m;
        }
};

class npAVGWINDOW: public npNODE { // aritmetic mean
        std::vector<uint32_t>    _window;
    public:
        npAVGWINDOW(size_t n){
            for(int i=0;i<n;i++) _window.push_back(0);
            _window.shrink_to_fit();
        }

        msg operator()(msg m) override {
            rotate(_window.begin(),++_window.begin(),_window.end());
            _window.pop_back();
            _window.push_back(m.load);
            uint32_t sigma=0;
            for(auto const& i:_window) sigma+=i;
            m.load=sigma / _window.size();
            return m;
        }
};

class npDELTAREPEAT: public npNODE {
        unsigned long   T=0;
        uint32_t        _f;
        H4_TIMER        _rpt=nullptr;
        uint32_t        _kravitz;
    public:    
        npDELTAREPEAT(uint32_t f): _f(f){}

        msg operator()(msg m) override;
};

class npFILTER: public npNODE {
        uint8_t _f;
    public:
        npFILTER(uint8_t filter): _f(filter){}

        msg operator()(msg m) override {
            m.halt=m.load!=_f;
            return m;
        }
};

class npFLIPFLOP: public npNODE {
        bool            _ff=false;
    public:
        msg operator()(msg m) override {
            m.load=_ff=!_ff;
            return m;
        }
};

class h4pEncoderAuto;
class npLIMITER: public npNODE {
        bool        _wrap;
        int         _value=0;
        int         _vMin,_vMax,_vInc;

        int         _clip(int v);
        void        _set(int v);
        h4pEncoderAuto*     _owner;
    public:
        npLIMITER(h4pEncoderAuto* owner,int vMin,int vMax,int vInc=1,int vSet=50,bool wrap=false): _owner(owner),_vMin(vMin),_vMax(vMax),_vInc(vInc),_wrap(wrap){ setPercent(vSet); }

        msg operator()(msg m) override { 
            m.load=_value=_clip(m.load);
            m.timer=true; // frig to allow valuediff
            return m;
        }

        void    center(){ _set(_vMin+((_vMax - _vMin)/2)); }
        int     getValue(){ return _value; }
        void    setPercent(int v){ _set(_vMin+(((_vMax - _vMin)*v)/100)); }
        void    setToMax(){ _set(_vMax); }
        void    setToMin(){ _set(_vMin); }
        void    setValue(int v){ _set(_clip(v)); }
};

class npPASSDELTA: public npNODE {
        unsigned long   _T=0;
        H4P_MSG_FLOW    _f;
    public:
        npPASSDELTA(H4P_MSG_FLOW f=[](msg m){ return m; }): _f(f){}

        msg operator()(msg m) override {
            if(_T){
                m.load=m.time - _T;
                _T=0;
                return _f(m); // publish(m,m.load);
            } else {
                _T=m.time;
                m.halt=true;
                return m;
            }
        };
};

class npPASSTHRU: public npNODE {
        uint8_t     _pA;
    public:
        npPASSTHRU(uint8_t pA): _pA(pA){}

        msg operator()(msg m) override;
};

class npPASSTIMER: public npNODE {
    public:
        msg operator()(msg m) override { m.halt=!m.timer; return m; }
};

class npPUBLISHCYCLE: public npNODE {
        uint32_t        _n; // umber of cycles
    public:    
        npPUBLISHCYCLE(uint32_t n): _n(n){}

        msg operator()(msg m) override {
            auto x=(m.sigE)%_n;
            return publish(m,x ? x:_n);
        };
};

class npPUBLISHDELTA: public npPASSDELTA {
    public:    
        npPUBLISHDELTA(): npPASSDELTA([](msg m){ return publish(m,m.load); }){}
};

class  npPUBLISHONOFF: public npNODE {
    public:
        msg operator()(msg m) override;
};

class  npPUBLISHSIGE: public npNODE {
    public:
        msg operator()(msg m) override { return publish(m,m.sigE); }
};

class  npPUBLISHVALUE: public npNODE {
    public:
        msg operator()(msg m) override { return publish(m,m.load); }
};

class  npROLLINGAVERAGE: public npNODE {
        int             _cma=0;
    public:
        msg operator()(msg m) override {
            auto cma=((_cma * m.sigE) + m.load)/(m.sigE+1);
            m.load=_cma=cma;
            return m;
        }
};
class  npROTARYDECODE: public npNODE {
    public:
            uint8_t         _pA;
            uint8_t         _pB;
            int8_t          _roti[16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0}; // valid(+-1) and invalid(0) rotation states
            uint8_t         _AB=0x03; // or 00 if active low!
            int             _val=0;
            bool            _sord;

        npROTARYDECODE(uint8_t pA,uint8_t pB,bool sord=true): _pA(pA),_pB(pB),_sord(sord){}
        msg operator()(msg m) override;
};

class  npSHOW: public npNODE {
    public:
        msg operator()(msg m) override {
            Serial.printf("  npSHOW L=%d P=%u T=%lu D=%u H=%d CLK=%d sigE=%u\n",m.load,m.phy,m.time,m.delta,m.halt,m.timer,m.sigE);
            return m;
        }
};

class npSMOOTH: public npNODE {
    protected:
                uint32_t        _ms; // amount of flatline before bouncing deemed over
                H4_TIMER        T=nullptr;
                uint32_t         _kravitz;

        virtual H4_TIMER        _startTimer(uint8_t p);
                void            _stopTimer();
                void            _restartTimer(uint8_t p);
    public:    
        npSMOOTH(uint32_t ms):  _ms(ms){}

        virtual msg operator()(msg m) override;
};

class npSTAGEMANAGER: public npNODE {
        H4PM_STAGE_MAP  _sm;
        int             _stage=0;
    public:
        npSTAGEMANAGER(H4PM_STAGE_MAP sm): _sm(sm){}

        msg operator()(msg m) override;
};

class npTHRESHOLD: public npNODE { // can we tidy / coalesce?
        int             _lim=0;
        H4PM_COMPARE    _cmp;
    public:
        npTHRESHOLD(int lim,H4PM_COMPARE cmp): _cmp(cmp),_lim(lim){}

        msg operator()(msg m) override {
            m.halt=!_cmp(m.load,_lim);
            return m;
        }
};
class npLOPASS: public npTHRESHOLD {
    public:
        npLOPASS(int lim): npTHRESHOLD(lim,H4PM_LESS){}
};
class npHIPASS: public npTHRESHOLD {
    public:
        npHIPASS(int lim): npTHRESHOLD(lim,H4PM_GREATER){}
};
class npNOTCH: public npNODE {
            int         _lo;
            int         _hi;
    public:
        npNOTCH(int lo,int hi): _lo(lo),_hi(hi){}

        msg operator()(msg m) override {
            m.halt=!(m.load < _lo || m.load > _hi);
            return m;
        }
};
class npBANDPASS: public npNODE {
            int         _lo;
            int         _hi;
    public:
        npBANDPASS(int lo,int hi): _lo(lo),_hi(hi){}

        msg operator()(msg m) override {
            m.halt=!(m.load > _lo && m.load < _hi);
            return m;
        }
};

class npTRIGGER: public npSMOOTH {
    public:
        npTRIGGER(uint32_t ms):npSMOOTH(ms){}

        msg operator()(msg m) override;
};

class npUPDATEINT: public npNODE {
        int&        _i;
    public:    
        npUPDATEINT(int& I): _i(I){}

        msg operator()(msg m) override;
};

class npUPDATEGLOBAL: public npNODE {
        std::string        _gv;
    public:    
        npUPDATEGLOBAL(const std::string& name): _gv(name){ h4p.gvSetInt(name,0,false); }

        msg operator()(msg m) override;
};

class npVALUEDIFF: public npNODE {
        int             _L0=0;
        int             _h;
    public:
        npVALUEDIFF(uint32_t h=0): _h(h){}
        
        msg operator()(msg m) override {
            int diff=m.load - _L0;
            diff=diff < 0 ? -diff:diff; // cheap n cheesy, breaks @ 32(31!)-bit limit...I know :)
            if(diff > _h){
                _L0=m.load;
                m.halt=false;
            } else m.halt=true;
            return m;
        }
};