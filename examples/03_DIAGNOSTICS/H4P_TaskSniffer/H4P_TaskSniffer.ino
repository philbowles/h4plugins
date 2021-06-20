/*
 *     YOU MUST SET #define H4P_SYS_LEVEL 2
 *     AND #define H4_HOOK_TASKS 1 in h4.h in the H$ library
 *     IN config_plugins.h!!!
 */
#define H4P_VERBOSE 1

#define SMALL_Q       10
#define Q_WARN_PCENT  50

#include<H4Plugins.h>
H4_USE_PLUGINS(115200,SMALL_Q,false) // Serial baud rate, Q size, SerialCmd autostop

H4P_TaskSniffer h4ts;

H4_INT_MAP myTaskNames{
    {1,"UF25"},
    {2,"UF30"},
    {3,"UF35"}, // don't have to be in order (but why not?)
    {4,"UF40"}, // don't have to be used (but why not?)
    {5,"UF50"},  // Keep them below 50 - IDs 50-99 are used by the system
    {6,"UF60"},
    {7,"UF70"},
    {8,"UF80"}
};

H4P_EventListener qtips(H4PE_SYSWARN,[](const std::string& svc,H4PE_TYPE t,const std::string& msg){
    // comes in as Q,1,N or Q,0,N
    std::vector<std::string> vs=split(msg,",");
    if(vs[0]=="Q"){
        Serial.printf("Q capacity is %u\n",SMALL_Q);
        Serial.printf("Q current size is %u\n",PARAM_INT(2));
        Serial.printf("Warning when size > %u\n",(SMALL_Q*Q_WARN_PCENT)/100);

        if(PARAM_INT(1)) Serial.println("Warning, Will Robinson - low Q!!!"); // See 1960s TV SciFi series "Lost in Space" :)
        else Serial.println("Disaster Averted"); 
    }
});

H4P_QueueWarn h4qw(50); // call qIsLow when free Q drops below 50%

void h4setup() { // H4 constructor starts Serial
    Serial.println("H4P_TaskSniffer example v"H4P_VERSION);
    H4::addTaskNames(myTaskNames);
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