#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) 

npPUBLISHONOFF enc2onoff;

class npENCODER10: public npNODE {
    public:    
        msg operator()(msg m) override {
          m.load=m.load < 1 ? 0:1;
          return enc2onoff(m);
        }
};

H4P_BinarySwitch bs(LED_BUILTIN,H4P_ASSUMED_SENSE);
h4pEncoder rotary(D1,D2,INPUT,ACTIVE_HIGH,true, new npENCODER10);