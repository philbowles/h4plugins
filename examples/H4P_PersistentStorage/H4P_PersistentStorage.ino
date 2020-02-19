#include<H4Plugins.h>
H4_USE_PLUGINS

H4 h4(115200); //auto-start Serial @ 115200, default Q size=20 

void onChange(const string& name,const string& value){
    Serial.printf("ITEM %s changed to %s\n",CSTR(name),CSTR(value));
}
H4P_SerialCmd h4sc;
H4P_PersistentStorage h4ps(onChange);

void h4setup() {
    h4ps.setstring("secret","life, the universe and everything");
    h4ps["peasy"]="easy"; //can also set string values like this
    string easy=h4ps["peasy"]; // or get them like this
    easy=h4ps.getstring("peasy"); // and this
    Serial.printf("Using H4P_PersistentStorage is %s\n",CSTR(h4ps["peasy"]));

    if(h4ps.exists("answer")){
        Serial.printf("What is the secret of %s?\n",CSTR(h4ps["secret"]));
        h4ps.setint("answer",42); // no short way to handle integers
        Serial.println("send h4/reboot to find out");
    }
    else {
        Serial.printf("Apparently the secret of %s is %d\n",CSTR(h4ps["secret"]),h4ps.getint("answer"));
        h4ps.inc("answer"); // increments the value: can also dec it
        Serial.println("send h4/reboot repeatedly to see increasingly WRONG answer");
    }
}
