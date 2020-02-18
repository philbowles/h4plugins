#ifndef ARDUINO_ARCH_STM32
    #ifdef ARDUINO_ARCH_ESP8266
        #include<ESP8266WiFi.h>
        #include<ESP8266mDNS.h>
        #include<ArduinoOTA.h>
        #include<ESPAsyncTCP.h>
        #include<ESPAsyncUDP.h>
    #else
    //    #pragma message("ESP32 compile with AsyncTCP");
        #include<WiFi.h>
        #include <AsyncTCP.h>
        #include <AsyncUDP.h>
    #endif
    #include<DNSServer.h>
    #include<ESPAsyncWebServer.h>
#else
    #define H4P_NO_WIFI
#endif