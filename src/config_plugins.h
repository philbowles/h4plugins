/*
 MIT License

Copyright (c) 2020 Phil Bowles <H48266@gmail.com>
   github     https://github.com/philbowles/H4
   blog       https://8266iot.blogspot.com
   groups     https://www.facebook.com/groups/esp8266questions/
              https://www.facebook.com/H4-Esp8266-Firmware-Support-2338535503093896/


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
#pragma once

#define H4P_VERSION "3.1.1"

// Make sure you read the documentation and have arrangements in place for using AP mode :)
// If you don't have the H4P_WiFiAP.cpp file, the linker will fail.
#define H4P_USE_WIFI_AP         0
/*
        DIAGNOSTICS
        comment out H4P_LOG_MESSAGES to prevent any Serial output of  H4PE_MSG EVENTs
        Increases performance, reduces binary size, increases free heap.
        
        H4P_SYS_LEVEL sets which of the H4PE_SYSxxxx messages get seen / ignored
        0 = show H4PE_SYSFATAL only (N.B. These cannot be turned off)
        1 = show H4PE_SYSFATAL, H4PE_SYSWARN
        2 = show H4PE_SYSFATAL, H4PE_SYSWARN, H4PE_SYSINFO

        but ...
        
        **********************************************************************
        YOU MUST H4P_LOG_MESSAGES DEFINED, H4P_SYS_LEVEL set to 2 AND
        INCLUDE ALL SERIAL MONITOR OUTPUT WHEN SUBMITTING BUG REPORTS
        ***********************************************************************

        H4P_UI_HEALTH = 1 adds Heap,Q,nLoops to UI when using heartbeat
*/
#define SANITY                  1
#define H4P_LOG_MESSAGES        1
#define H4P_SYS_LEVEL           2
#define H4P_UI_HEALTH           0
/*
        HARDWARE ASSUMPTIONS

        Most boards have a builtin LED where they have this will be defined by LED_BUILTIN
        of you want to use a different one, change H4P_ASSUMED_LED to your chosen GPIO (NOT ARDUINO DIGITAL PIN NUMBER!!!)

        Also it is very common for builtin LEDs to be "Active Low" meaning it is ON when the pin is driven to GND = "LOW" = 0
        if yours is Active HIGH (ON when the pin id driven to Vcc = HIGH = 1 ) then change H4P_ASSUMED_SENSE to 1

        Finally, if you want to correctly visualise the LED on the web UI give it a color: the basic assumption is blue;

        These affect the way wifi, mqtt and multifunction buttons do their signalling
*/
enum H4PM_SENSE:uint8_t {
    ACTIVE_LOW,
    ACTIVE_HIGH
};

enum H4P_UILED_COLOR {
    H4P_UILED_RED,
    H4P_UILED_ORANGE,
    H4P_UILED_YELLOW,
    H4P_UILED_GREEN,
    H4P_UILED_BLUE,
    H4P_UILED_BI
};

#ifdef LED_BUILTIN
    #define H4P_ASSUMED_LED      LED_BUILTIN
    #define H4P_ASSUMED_SENSE     ACTIVE_LOW
    #define H4P_ASSUMED_COLOR H4P_UILED_BLUE
#endif

#define H4P_WEBSERVER_PORT      80
/*
            TWEAKABLES
*/
//#define H4P_CMDLINE_FLASHERS     0

#define H4P_REPLY_BUFFER       256
#define H4PE_BUFFER            256
#define H4P_REBOOT_DELAY      1000

//#define H4FC_MORSE_SUPPORT

#define H4MQ_RETRY           20000

#define H4P_UDP_JITTER         250
#define H4P_UDP_REFRESH     300000
#define H4P_UDP_REPEAT           2

#define H4MF_SLOW              250
#define H4MF_MEDIUM            125
#define H4MF_REBOOT           2000
#define H4MF_FACTORY          5000

#define H4WF_AP_RATE           500
#define H4WF_OTA_RATE         1500
#define H4WF_EVT_TIMEOUT     30000

#define H4P_GK_SCAVENGE      30000
#define H4P_GK_STAGGER           1

#define H4P_IPPD_RATE        10000
#define H4P_PJ_SPREAD            5

#define H4P_PJ_LO (H4P_IPPD_RATE - (H4_JITTER_LO * H4P_PJ_SPREAD))
#define H4P_PJ_HI (H4P_IPPD_RATE + (H4_JITTER_HI * H4P_PJ_SPREAD))

#define H4P_RUPD_STRETCH         5

#define H4PM_GRANULARITY        50

#ifdef ARDUINO_ARCH_ESP8266
    #define H4P_THROTTLE_LO       6000
    #define H4P_THROTTLE_HI       9000
#else
    #define H4P_THROTTLE_LO     100000
    #define H4P_THROTTLE_HI     125000
#endif

#define H4P_TIME_HOLDOFF          2000
#define H4P_TIME_RESYNC        3600000

constexpr const char* h4pTag(){ return "H4P"; }
constexpr const char* httpTag(){ return "http://"; }
constexpr const char* rootTag(){ return "upnp:rootdevice"; }
constexpr const char* rupdTag(){ return "updateURL"; }
constexpr const char* glob(){ return "/glob"; }