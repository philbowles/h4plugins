/*
 *     YOU MUST SET #define H4P_SYS_LEVEL 2
 *     IN config_plugins.h!!!
 */
#define H4P_VERBOSE 1

#define SMALL_Q       10
#define Q_WARN_PCENT  50

#include<H4Plugins.h>
H4_USE_PLUGINS(115200,SMALL_Q,false) // Serial baud rate, Q size, SerialCmd autostop

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
    Serial.println("H4P_QueueWarn example v"H4P_VERSION);
    h4qw.info(); // show limits
//
//  we need a lot of things in the queue
//
    h4.once(30000,[](){ Serial.println("30 seconds later"); });
    h4.once(40000,[](){ 
      Serial.println("40 seconds later");
      Serial.println("Add more jobs to generate 2nd Warning");
      h4.once(25000,[](){ Serial.println("65 seconds later"); }); // 40 + 25
      h4.once(35000,[](){ Serial.println("75 seconds later"); }); // 40 + 35
    });
    h4.once(50000,[](){ Serial.println("50 seconds later"); });
    h4.once(60000,[](){ Serial.println("60 seconds later"); });
    h4.once(70000,[](){ Serial.println("70 seconds later"); });
    h4.once(80000,[](){ 
      Serial.println("80 seconds later");
      // What follows is NOT a good idea. it reruns h4setup, causing an infinte loop
      // It is done purely to show the effect of changing the warning limit "in-flight"
      // subsequent iterations will produce no warnings
      Serial.println("WARNING! DO NOT TRY THIS AT HOME!");
      h4qw.pcent(100); // reset to max - no warnings second time around
      h4setup();      
    });
}