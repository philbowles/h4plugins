
/*
 MIT License

Copyright (c) 2019 Phil Bowles <h4plugins@gmail.com>
   github     https://github.com/philbowles/H4P_Sunrise
   blog       https://8266iot.blogspot.com     
   groups     https://www.facebook.com/groups/esp8266questions/
              https://www.facebook.com/H4P_Sunrise-Esp8266-Firmware-Support-2338535503093896/
                			  

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifdef ARDUINO_ARCH_ESP8266
#include<H4P_Sunrise.h>
#include<H4P_SerialCmd.h>
//#include<H4P_BinaryThing.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

void H4P_Sunrise::_hookIn(){ 
    REQUIRE(time);
    DEPEND(wifi);
}

void H4P_Sunrise::_parse(const string& s){
//    Serial.printf("PARSE T %s\n",CSTR(s));
    vector<string> parts=split(replaceAll(s,"\"",""),":");
//    dumpvs(parts);
    string sex=parts[3];
    sex.pop_back(); // lose the M
    int H=atoi(CSTR(parts[1]))+(sex.back()=='P' ? 12:0);
    sex.pop_back();
    _cb[parts[0]]=stringFromInt(H,"%02d")+":"+parts[2]+":"+sex;
}

void H4P_Sunrise::_start(){
    if(!_cb.count("sunrise")){
        std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

        client->setFingerprint("62 ff 6a d5 37 aa 52 15 e4 3f 11 02 7c 88 d9 7e 7b 5c a0 2f");

        HTTPClient https;

        if (https.begin(*client, String("https://api.sunrise-sunset.org/json?")+CSTR(_latlong))) {  // HTTPS
        int httpCode = https.GET();

        if (httpCode > 0) {
            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();
            string chop=CSTR(payload.substring(12,60));
            vector<string> chop2=split(chop,","); // "sunrise":"5:11:52 AM" 
            _parse(chop2[0]);
            _parse(chop2[1]);
            h4tk.daily("00:00",ON,[this](bool b){
                _cb.erase("sunrise");
                _start();
            });
            }
        } else H4EVENT("sset FAIL %d",httpCode);
        https.end();
        }
    } //else H4EVENT("ALREADY GOT SS %s",CSTR(_cb["sunrise"]));
}
#endif // esp8266 only