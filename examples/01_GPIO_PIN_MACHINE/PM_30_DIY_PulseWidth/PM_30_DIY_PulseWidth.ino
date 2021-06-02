#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false)

#define USER_BTN  0

class DIYpulseWidth: public h4pGPIO {
    public:
        DIYpulseWidth(uint8_t p,uint8_t m,H4GM_SENSE s):
            h4pGPIO(p,m,s,{
              new npPASSDELTA,
              npPublishValue
            }){
        }
};

H4P_EventListener gpio(H4PE_GPIO,[](const std::string& pin,H4PE_TYPE t,const std::string& msg){
    int p=atoi(pin.c_str());
    int v=atoi(msg.c_str());
    Serial.printf("P%d Delta = %d\n",p,v);
});

DIYpulseWidth pw(USER_BTN,INPUT_PULLUP,ACTIVE_LOW);