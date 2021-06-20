/*
 *     YOU MUST SET #define H4P_SYS_LEVEL 2
 *     IN config_plugins.h!!!
 */
#define H4P_VERBOSE 1
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

std::vector<void*> chunks;

H4P_SerialLogger h4sl;
H4P_HeapWarn h4hw(50); // call heapIsLow when free Heap drops below 50% of its initial value
/*
    Open Serial monitor and try typing any of the following:
    
    * h4/svc/info/hwrn
    * h4/hwrn/pcent/< some sensible percentage>
*/
H4P_EventListener uriah(H4PE_SYSWARN,[](const std::string& svc,H4PE_TYPE t,const std::string& msg){
    // comes in as Heap,1,N or Heap,0,N
    std::vector<std::string> vs=split(msg,",");
    if(vs[0]=="Heap"){
        Serial.printf("Current heap size is %d\n",PARAM_INT(2));
        if(PARAM_INT(1)) Serial.println("Warning, Will Robinson - low heap!!!"); // See 1960s TV SciFi series "Lost in Space" :)
        else Serial.println("Disaster Averted");
    }
});

void h4setup() { // H4 constructor starts Serial
    Serial.println("H4P_HeapWarn example v"H4P_VERSION);
    Serial.println("This WILL crash!");
//
//  we need to chew up the heap in 1k chunks
//
    h4.every(500,[](){
      chunks.push_back(malloc(1024));
      Serial.printf("Heap=%d\n",_HAL_freeHeap());
    });    
    h4.once(30000,[](){ 
      Serial.println("30 seconds later, claw it back");
      for(auto c:chunks) free(c);
      Serial.printf("Heap=%d\n",_HAL_freeHeap());
    });
}