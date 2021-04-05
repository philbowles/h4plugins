#define H4P_VERBOSE 1
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop
//
// all svc and msg fields are strings. When the event type is H4PE_GPIO
//  you pretty much always need the integer pin number (from svc) and the integer value (from msg):
//  if you have many gpio pins then you will probably need to switch(pin){ ... } and breaking out
//  the gpio handling also prevents too many "flying ducks" :)
//
//  H4P_GPIO_CONNECTOR in the main Event handler will do the int conversions and re-route
//  all GPIO messages here:
//
void onGPIO(int pin,int value){
  Serial.printf("Pin %d changed value to %d\n",pin,value);
/*
 * Typically:
 * 
 * switch(pin){
 *  case USER_BUTTON_1:
 *    if(value == ON){
 *    
 *  ... etc
 * }
 */
}

h4pOutput builtin(LED_BUILTIN,H4P_ASSUMED_SENSE,OFF);

void h4pGlobalEventHandler(const string& svc,H4PE_TYPE t,const string& msg){
  switch(t){
    H4P_FUNCTION_ADAPTER_II(GPIO);
  }   
}

void h4setup(){
  h4.every(5000,[]{ builtin.toggle(); });
}