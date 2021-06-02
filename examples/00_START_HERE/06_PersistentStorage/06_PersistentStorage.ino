  #include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) // Serial baud rate, Q size, SerialCmd autostop

/* Uncomment this to see what's happening "under the hood"
H4P_EventListener chchchanges(H4PE_GVCHANGE,[](const std::string& svc,H4PE_TYPE t,const std::string& msg){ 
  Serial.printf("USER %s=%s\n",CSTR(svc),CSTR(msg)); 
});
*/
void h4setup() {
    // show system variables: AVOID THESE NAMES!
    h4p.info();
 
    h4p.gvSetstring("secret","life, the universe and everything",false); // false makes it temporary: doesn't get saved
    h4p["peasy"]="easy"; //can also set string values like this
    std::string easy=h4p["peasy"]; // or get them like this
    easy=h4p.gvGetstring("peasy"); // and this
    Serial.printf("Using H4Plugins is %s\n",CSTR(h4p["peasy"]));
 
    if(!h4p.gvExists("answer")){
        Serial.printf("What is the secret of %s?\n",CSTR(h4p["secret"]));
        h4p.gvSetInt("answer",42,true);
        Serial.println("send h4/reboot to find out");
    }
    else {
        Serial.printf("Apparently the secret of %s is %d\n",CSTR(h4p["secret"]),h4p.gvGetInt("answer"));
        h4p.gvInc("answer"); // increments the value: can also dec it
        Serial.println("send h4/reboot repeatedly to see increasingly WRONG answer");
        Serial.println("send h4/factory to restart from scratch");
    }
}