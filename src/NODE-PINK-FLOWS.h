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
#define PIPELINE(n,x) static_cast<x*>(_pA->_pipeline[n]) 
//
struct h4pAnalogAverage: public h4pGPIO { h4pAnalogAverage(uint8_t p,uint32_t f,uint32_t n,npNODE* d=new npPUBLISHVALUE); };

struct h4pAnalogAvgChanged: public h4pGPIO { h4pAnalogAvgChanged(uint8_t p,uint32_t f,uint32_t n,uint32_t h=0,npNODE* d=new npPUBLISHVALUE); };

struct h4pAnalogAvgRolling: public h4pGPIO { h4pAnalogAvgRolling(uint8_t p,uint32_t f,npNODE* d=new npPUBLISHVALUE); };

struct h4pAnalogAvgWindow: public h4pGPIO { h4pAnalogAvgWindow(uint8_t p,uint32_t f,uint32_t n,npNODE* d=new npPUBLISHVALUE); };

struct h4pAnalogPolled: public h4pGPIO { h4pAnalogPolled(uint8_t p,uint32_t f,uint32_t h=0,npNODE* d=new npPUBLISHVALUE); };

struct h4pAnalogThreshold: public h4pGPIO { h4pAnalogThreshold(uint8_t p,uint32_t f,H4PM_COMPARE cmp,uint32_t lim, uint32_t h=0,npNODE* d=new npPUBLISHVALUE); };

struct h4pAnalogTMP36: public h4pGPIO { h4pAnalogTMP36(uint8_t p,uint32_t f,uint32_t h=0,npNODE* d=new npPUBLISHVALUE); };

struct h4pCircular: public h4pGPIO { h4pCircular(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t dbTime,uint32_t N); };

struct h4pCounting: public h4pGPIO { h4pCounting(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t dbTime); };

struct h4pDebounced: public h4pGPIO { h4pDebounced(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t dbTime,npNODE* d=new npPUBLISHVALUE); };

class h4pEncoder_Half: public h4pGPIO {
    protected:
        virtual void _announce() override;
    public:
        h4pEncoder_Half(uint8_t p,uint8_t m,H4PM_SENSE s,npFLOW f);
};
class h4pEncoder {
    public:
        h4pGPIO*     _pA;
        h4pEncoder(uint8_t pA,uint8_t pB,uint8_t m=INPUT,H4PM_SENSE s=ACTIVE_HIGH,bool sord=true,npNODE* d=new npPUBLISHVALUE);
};

class h4pEncoderAuto {
        friend class npLIMITER;
        h4pGPIO*     _pA;
    public:
        h4pEncoderAuto(uint8_t pA,uint8_t pB,uint8_t m=INPUT,H4PM_SENSE s=ACTIVE_HIGH,int vMin=0,int vMax=100,int vInc=1,int _vSet=50,bool wrap=false,npNODE* d=new npPUBLISHVALUE);

        void    center(){ PIPELINE(1,npLIMITER)->center(); }
        int     getValue(){ return PIPELINE(1,npLIMITER)->getValue(); }
        void    setPercent(int v){ PIPELINE(1,npLIMITER)->setPercent(v); }
        void    setToMax(){ PIPELINE(1,npLIMITER)->setToMax(); }
        void    setToMin(){ PIPELINE(1,npLIMITER)->setToMin(); }
        void    setValue(int v){ PIPELINE(1,npLIMITER)->setValue(v); }
};

struct h4pFiltered: public h4pGPIO { h4pFiltered(uint8_t p,uint8_t m,H4PM_SENSE s,uint8_t filter,npNODE* d=new npPUBLISHVALUE); };

struct h4pLatching: public h4pGPIO { h4pLatching(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t dbTime,npNODE* d=new npPUBLISHVALUE); };

class h4pMultistage: public h4pGPIO {
    protected:
        virtual void        _announce() override {}
    public:
        h4pMultistage(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t dbTimeMs,H4PM_STAGE_MAP stageMap,npNODE* d=new npPUBLISHVALUE);
};

class h4pMultifunctionButton: public h4pMultistage {
        std::string          _me="mfnb";  // ease of interop w/other event handlers
    protected:
            void        _handleEvent(const std::string& s,H4PE_TYPE t,const std::string& m);
    public:
        h4pMultifunctionButton(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t t);
};

class h4pOutput: public h4pGPIO {
    public:
        h4pOutput(uint8_t p,H4PM_SENSE s,uint8_t initial=OFF,uint8_t c=H4P_UILED_RED,npNODE* d=new npPUBLISHVALUE);

    virtual bool        isAnalogInput() override { return false; }
    virtual bool        isAnalogOutput() { return _HAL_isAnalogOutput(_p); }
    virtual bool        isOutput() override { return true; }
            void        logicalWrite(bool b){ turn(b); }
            void        turnOff(){ turn(false); }
            void        turnOn(){ turn(true); }
            void        toggle(){ turn(!_normalise(_r)); }
            void        turn(bool b);
};

struct h4pPolled: public h4pGPIO { h4pPolled(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t f,npNODE* d=new npPUBLISHVALUE); };

struct h4pRaw: public h4pGPIO { h4pRaw(uint8_t p,uint8_t m,H4PM_SENSE s,npNODE* d=new npPUBLISHVALUE); };

struct h4pRepeating: public h4pGPIO { h4pRepeating(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t dbTime,uint32_t f); };

class h4pRetriggering: public h4pGPIO {
    protected:
        virtual void        _announce() override;
    public:
        h4pRetriggering(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t t,npNODE* d=new npPUBLISHVALUE);
};
struct h4pTactless: public h4pGPIO { h4pTactless(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t dbTime,npNODE* d=new npPUBLISHVALUE); };

struct h4pTimed: public h4pGPIO { h4pTimed(uint8_t p,uint8_t m,H4PM_SENSE s,uint32_t dbTime); };
//
// popular multi-use
//
extern npFILTER*        npFilterINACTIVE;
extern npPASSTIMER*     npPassTimer;
extern npPUBLISHVALUE*  npPublishValue;
extern npSHOW*          npShow;