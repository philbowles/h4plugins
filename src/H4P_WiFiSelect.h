#ifndef ARDUINO_ARCH_STM32
    #ifdef ARDUINO_ARCH_ESP8266
        #include<ESP8266WiFi.h>
        #include<ESP8266mDNS.h>
        #include<ESPAsyncTCP.h>
        #include<ESPAsyncUDP.h>
    #else
        #include<WiFi.h>
        #include <AsyncTCP.h>
        #include <AsyncUDP.h>
    #endif
    #include<DNSServer.h>
    #include<ArduinoOTA.h>
#else
    #define H4P_NO_WIFI
#endif