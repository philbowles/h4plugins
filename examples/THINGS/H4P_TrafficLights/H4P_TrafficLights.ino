#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop
/* 
 *   HARDWARE REQUIRED: 
 *   
 *   "Tact" switch on GPIO0 that pulls it to GND when pressed
 *   
 *   Three LEDS, preferably   
 *   RED on D6(GPIO12), ORANGE on D7(GPIO13) GREEN on D8(GPIO15)
 *   
 *   LED plus current limiting resistor on each pin required
 *    connect Vcc ------^^^--------D|----> GPIO
 *                   resistor     LED
 *    and define active LOW
 *    
 *    OR:
 *    
 *    connect GPIO ------^^^--------D|----> GND
 *                   resistor     LED
 *    and define active HIGH
 */
#define RED_LIGHT    D6
#define AMBER_LIGHT  D7
#define GREEN_LIGHT  D8

H4P_GPIOManager gpio;
/////////////////////////////////////////////////////////////////////////////////////
//
//  This is my "thing": a UK pattern Traffic light sequencer.
//
////////////////////////////////////////////////////////////////////////////////////  
int T1=5000; // see the phasing.xls spreadsheet for choosing these numbers
int T2=1250;
//
// A utility function to switch on led, t mSec later switch it off, then execute f
//
void switchOn(uint8_t led,uint32_t t,H4_FN_VOID f=[](){}){
  gpio.logicalWrite(led,ON);
  h4.once(t,[led](){ gpio.logicalWrite(led,OFF); },f); // run f as soon as led goes off    
}

void trafficLights(bool onoff){ // this is  the actual "thing" function
  static H4_TIMER running=nullptr;
  if(onoff){
    switchOn(RED_LIGHT,T1+T2);
    h4.once(T1,[](){
      switchOn(AMBER_LIGHT,T2,[](){ // AMBER stays on for T2, and when it goes off...
        switchOn(GREEN_LIGHT,T1,[](){ // light GREEN for T1, and when it goes off...
          switchOn(AMBER_LIGHT,T2); }); // light AMBER for T2 when done,rinse and repeat
          });
      }); 
    if(!running) running=h4.every(2*(T1+T2),[](){ trafficLights(true); }); // rerun yourself
  }
  else { // stop everything
    h4.cancelAll();
    running=nullptr;
    gpio.logicalWrite(RED_LIGHT,OFF);
    gpio.logicalWrite(AMBER_LIGHT,OFF);
    gpio.logicalWrite(GREEN_LIGHT,OFF);
  }
}
///////////////////////////////////////////////////////////////////////////////
//
//    That's all the code for the "thing" , all else is the same as before
//
//////////////////////////////////////////////////////////////////////////////
//
H4P_BinaryThing h4onof(trafficLights);
H4P_MultiFunctionButton h4mfb(BUTTON_BUILTIN,INPUT,ACTIVE_LOW,15);

void h4setup(){
  gpio.Output(RED_LIGHT,ACTIVE_HIGH,OFF);
  gpio.Output(AMBER_LIGHT,ACTIVE_HIGH,OFF);     
  gpio.Output(GREEN_LIGHT,ACTIVE_HIGH,OFF);
  
}