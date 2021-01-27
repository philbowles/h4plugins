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

#define H4P_VERSION "1.0.0"
/*
        DIAGNOSTICS
        comment out H4P_LOG_EVENTS to prevent any logging by EVENT( whatever ) messages
        Increases performance, reduces binary size, increases free heap, but ...
        
        **********************************************************************
        YOU MUST H4P_LOG_EVENTS DEFINED AND INCLUDE ALL SERIAL MONITOR OUTPUT
        WHEN SUBMITTING BUG REPORTS
        ***********************************************************************
*/
#define H4P_LOG_EVENTS      1
#define SANITY              1
/*
        HARDWARE ASSUMPTIONS

        Most boards have a builtin LED where they have this will be defined by LED_BUILTIN
        of you want to use a differetn one, change H4P_SIGNAL_LED to your chosen GPIO (NOT ARDUINO DIGOTAL PIN NUMBER!!!)

        Also it is very common for builtin LEDs to be "Active Low" meaning it is ON when the pin is driven to GND = "LOW" = 0
        if yours is Active HIGH (ON when the pin id driven to Vcc = HIGH = 1 ) then change H4P_SIGNAL_SENSE to 1

        These affect the way wifi, mqtt and multfunction buttons do their signalling
        H4P_SIGNAL_TIMEBASE is the speed at which the signal pattern cycles - see the documentation for flashPattern API
*/
enum H4GM_SENSE:uint8_t {
    ACTIVE_LOW,
    ACTIVE_HIGH
};

#define H4P_SIGNAL_LED  LED_BUILTIN
#define H4P_SIGNAL_SENSE ACTIVE_LOW
#define H4P_SIGNAL_TIMEBASE     175


#define H4P_WEBSERVER_PORT      80
// Make sure you read the documentation and have arrangements in place for using AP mode :)
#define H4P_USE_WIFI_AP          0
/*
            TWEAKABLES
*/
#define H4P_REPLY_BUFFER       256
#define H4P_EVENT_BUFFER       256

//#define H4FC_MORSE_SUPPORT

#define H4MQ_RETRY           10000

#define H4P_UDP_JITTER         250
#define H4P_UDP_REFRESH     300000
#define H4P_UDP_REPEAT           2

#define H4MF_SLOW              250
#define H4MF_MEDIUM            125
#define H4MF_REBOOT           2000
#define H4MF_FACTORY          5000

#define H4WF_AP_RATE           500
#define H4WF_OTA_RATE         1500
#define H4P_ASWS_EVT_TIMEOUT 30000

#define H4P_IPPD_RATE         5000
#define H4P_PJ_SPREAD            3

#define H4P_PJ_LO (H4P_IPPD_RATE - (H4_JITTER_LO * H4P_PJ_SPREAD))
#define H4P_PJ_HI (H4P_IPPD_RATE + (H4_JITTER_HI * H4P_PJ_SPREAD))

#define H4P_RUPD_STRETCH         5

#define H4P_TIME_HOLDOFF      2000
#define H4P_TIME_RESYNC    3600000

constexpr const char* h4pTag(){ return "h4P"; }
constexpr const char* httpTag(){ return "http://"; }
constexpr const char* rootTag(){ return "upnp:rootdevice"; }