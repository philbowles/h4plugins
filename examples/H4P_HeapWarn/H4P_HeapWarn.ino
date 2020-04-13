#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop

void heapIsLow(bool inDanger){ 
    Serial.print("Current heap size is ");Serial.println(ESP.getFreeHeap());
    if(inDanger) Serial.println("Warning, Will Robinson - low heap!!!"); // See 1960s TV SciFi series "Lost in Space" :)
    else Serial.println("Disaster Averted"); 
}

H4P_SerialLogger h4sl;
H4P_HeapWarn h4hw(heapIsLow,50); // call heapIsLow when free Heap drops below 50% of its initial value
/*
    Open Serial monitor and try typing any of the following:
    
    * h4/svc/info/hwrn
    * h4/hwrn/pcent/< some sensible percentage>
*/

vector<void*> chunks;

void h4setup() { // H4 constructor starts Serial
    Serial.println("H4P_HeapWarn example v"H4P_VERSION);
    Serial.println("This WILL crash!");
    //h4hw.show(); // show limits
//
//  we need to chew up the heap in 0.5k chunks
//
    h4.every(500,[](){
      chunks.push_back(malloc(512));
      Serial.printf("Heap=%d\n",ESP.getFreeHeap());
    });    
    h4.once(30000,[](){ 
      Serial.println("30 seconds later, claw it back");
      for(auto c:chunks) free(c);
      Serial.printf("Heap=%d\n",ESP.getFreeHeap());
    });
}