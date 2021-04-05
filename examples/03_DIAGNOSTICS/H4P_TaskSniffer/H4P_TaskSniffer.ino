#define SMALL_Q       10
#define Q_WARN_PCENT  50

#define H4P_VERBOSE 1
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,SMALL_Q,false) // Serial baud rate, Q size, SerialCmd autostop

void qIsLow(bool inDanger){ 
  if(inDanger) {
    Serial.println("Warning, Will Robinson - low Q!!!"); // See 1960s TV SciFi series "Lost in Space" :)
    h4p.showQ();
  }
}

H4P_QueueWarn h4qw(qIsLow,50); // call qIsLow when free Q drops below 50%
H4P_TaskSniffer h4ts;
/*
    We are going to name our own tasks so that they are obvious in the dump.
    If we don't do this, all tasks wil appear as "ANON"

    The function below will be called by H4 when it finds one of your tasks

*/
const char* giveTaskName(uint32_t n){
  static H4_INT_MAP mydata={
    {1,"UF25"},
    {2,"UF30"},
    {3,"UF35"}, // don't have to be in order (but why not?)
    {4,"UF40"}, // don't have to be used (but why not?)
    {5,"UF50"},  // Keep them below 50 - IDs 50-99 are used by the system
    {6,"UF60"},
    {7,"UF70"},
    {8,"UF80"}
    };
  return mydata.count(n) ? mydata[n].c_str():"ANON";
}
// if you don't want task naming, just delete the above AND REMOVE ...,nullptr,<N> from the
//  tasks below!!!

void h4setup() { // H4 constructor starts Serial
        Serial.println("H4P_TaskSniffer example v"H4P_VERSION);
        h4qw.info(); // show limits
    //
    //  we need a lot of things in the queue
    //
        h4.once(30000,[](){ Serial.println("30 seconds later"); },nullptr,2); // "tag" this task as UF30
        h4.once(40000,[](){ 
          Serial.println("40 seconds later");
          h4.once(25000,[](){ Serial.println("65 seconds later"); },nullptr,1); // 40 + 25 "tag" as UF25
          h4.once(35000,[](){ Serial.println("75 seconds later"); },nullptr,3); // 40 + 35 "tag" as UF35
        },nullptr,4); // "tag" as UF40
        h4.once(50000,[](){ Serial.println("50 seconds later"); },nullptr,5);// "tag" as UF50
        h4.once(60000,[](){ Serial.println("60 seconds later"); },nullptr,6);// "tag" as UF60
        h4.once(70000,[](){ Serial.println("70 seconds later"); },nullptr,7);// "tag" as UF70
        h4.once(80000,[](){ Serial.println("80 seconds later"); },nullptr,8);// "tag" as UF80
        h4.once(90000,[](){ Serial.println("90 seconds later"); });// no tag: anon
        h4.once(95000,[](){ Serial.println("95 seconds later"); });// no tag: anon
}