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
#include<H4P_udpLogger.h>

void H4P_udpLogger::_hookIn(){ DEPEND(wifi); }

void H4P_udpLogger::_listenUDP(){
    if(_udp.listenMulticast(_ubIP, 1900)) {
        _udp.onPacket([this](AsyncUDPPacket packet) {
            if(ESP.getFreeHeap() < limit){
                h4.queueFunction(bind([this](string msg, IPAddress ip, uint16_t port) {
                    rps++;
                    if(rps > maxrate) maxrate=rps;
                    uint32_t heapnow=ESP.getFreeHeap();
                    uint32_t pcnow=heapnow*100/startheap;
                    int d=lastheap - heapnow;
                    if(abs(d) > delta) {
                        Serial.printf("******************* Heap %s %d bytes ******************\n",d<0 ? "GAIN":"LOSS",abs(d));
                        lastheap=heapnow;
                    }
                    Serial.printf("From %s:%d %u %u %u%% rate=%u/sec (max %u)\n",CSTR(ip.toString()),port,micros(),heapnow,pcnow,rps,maxrate);
                },stringFromBuff(packet.data(), packet.length()),packet.remoteIP(), packet.remotePort()),nullptr, H4P_TRID_UDPM);
            } //else Serial.printf("IGNORED %u > %u\n",ESP.getFreeHeap(),limit);
        });
    }
}

void H4P_udpLogger::_start(){
    Serial.printf("udp logger: start heap %u min Delta=%u notify below %u\n",startheap,delta,limit);
    _listenUDP();
    h4.every(1000,[this](){ rps=0; });
    _upHooks();
}