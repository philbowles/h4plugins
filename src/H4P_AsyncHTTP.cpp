/*
 MIT License

Copyright (c) 2020 Phil Bowles <h4plugins@gmail.com>
   github     https://github.com/philbowles/esparto
   blog       https://8266iot.blogspot.com     
   groups     https://www.facebook.com/groups/esp8266questions/
              https://www.facebook.com/Esparto-Esp8266-Firmware-Support-2338535503093896/
                			  

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

#include<H4P_AsyncHTTP.h>
#include<H4P_WiFi.h>

H4P_AsyncHTTP::H4P_AsyncHTTP(const std::string& name,uint32_t filter): H4Service(name,filter,false){
    depend<H4P_WiFi>(wifiTag());
    onHTTPerror([=](int e,int i){ _errorHandler(e,i); });
}

void H4P_AsyncHTTP::DELETE(const std::string& url,ARMA_FN_HTTP rx,const uint8_t* fingerprint,uint32_t phase){ if(_running) ArmadilloHTTP::DELETE(url,rx,fingerprint,phase); }
void H4P_AsyncHTTP::GET(const std::string& url,ARMA_FN_HTTP rx,const uint8_t* fingerprint,uint32_t phase){ if(_running) ArmadilloHTTP::GET(url,rx,fingerprint,phase); }
void H4P_AsyncHTTP::PATCH(const std::string& url,const VARK_NVP_MAP& fields,ARMA_FN_HTTP rx,const uint8_t* fingerprint,uint32_t phase){ if(_running) ArmadilloHTTP::PATCH(url,fields,rx,fingerprint,phase); }
void H4P_AsyncHTTP::POST(const std::string& url,const VARK_NVP_MAP& fields,ARMA_FN_HTTP rx,const uint8_t* fingerprint,uint32_t phase){ if(_running) ArmadilloHTTP::POST(url,fields,rx,fingerprint,phase); }
void H4P_AsyncHTTP::PUT(const std::string& url,const VARK_NVP_MAP& fields,ARMA_FN_HTTP rx,const uint8_t* fingerprint,uint32_t phase){ if(_running) ArmadilloHTTP::PUT(url,fields,rx,fingerprint,phase); }

void H4P_AsyncHTTP::globalsFromSimpleJson(ARMA_HTTP_REPLY r){ for(auto const& j:r.asSimpleJson()) h4p["usr_"+j.first]=j.second; }