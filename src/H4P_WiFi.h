#ifndef H4P_WIFI_H
#define H4P_WIFI_H

#ifdef ARDUINO_ARCH_ESP8266
    #include<ESP8266WiFi.h>
    #include<ESP8266mDNS.h>
    #include<ArduinoOTA.h>
    #include<ESPAsyncTCP.h>
    #include<ESPAsyncUDP.h>
    #include<DNSServer.h>

    #include<H4P_WiFi8266.h>
#elif defined ARDUINO_ARCH_ESP32
    #include<WiFi.h>
    #include<SPIFFS.h>
    #include <AsyncTCP.h>
    #include <AsyncUDP.h>
    #include<DNSServer.h>

    #include<H4P_WiFi32.h>
#else
    #define H4P_NO_WIFI
#endif

#endif