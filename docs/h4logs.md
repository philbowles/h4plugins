![H4P Logo](/assets/DiagLogo.jpg)

# Logging / Loggers

## Adds event logging to multiple destinations for H4 Universal Scheduler/Timer + Plugins.

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---

# What do they do?

H4Plugins loggers can send messages to a variety of destinations, depending on which logger you choose. There are currently 

* H4P_HttpMySQLLogger which sends logged events to a remote Webserver for storage in a MySQL database **NEW in v0.4.0**
  **N.B. H4P_HttpMySQLLogger is experimental and has [its own documentation page](h4mysql.md)**

* H4P_MQTTLogger sends log messages to MQTT Server **NEW in v0.3.4**
* H4P_MQTTHeapLogger a specialised H4P_MQTTLogger which periodically logs value of FreeHeap to MQTT **NEW in v0.3.4**
* H4P_MQTTQueueLogger a specialised H4P_MQTTLogger which periodically logs size of queue to MQTT **NEW in v0.4.0**
* H4P_SerialLogger which does exactly what it says on the tin

Writing your own logger is seriously easy (see below)

The important thing to note is that each logger is called in turn, thus you can log to several destinations at a time for a single message. The serial logger is a useful diagnostic aid, as it shows what is getting logged to other - less visible - destinations, e.g. remote servers.

Also note that the main interface `h4sc.logEvent("some message",...)` (which operates like `printf` with variable number of parameters) will simply do nothing if no loggers are installed. This allows it to be left in the code and "switched on or off" by commenting out the loggers, which can be flipped back in at a stroke for testing.

Better still. there is a macro EVENT("some message",...) which simply calls `h4sc.logEvent` but can be "compiled out" by removing the `#define H4P_LOG_EVENTS` entry in `H4PConfig.h`, then there is zero overhead.

Finally, most loggers have  `filter` parameter which allows the logger to operate on only certain event types:

```cpp
enum H4P_LOG_TYPE {
    H4P_LOG_H4=1, // reserved for H4 plugins own use
    H4P_LOG_SVC_UP=2,// internal H4 event on service UP
    H4P_LOG_SVC_DOWN=4,// internal H4 event on service DOWN
    H4P_LOG_CMD=8,// command from any source (see below)
    H4P_LOG_USER=16,// arbitrary message from user code
    H4P_LOG_DEPENDFAIL=32,// dependent plugin omitted by user
    H4P_LOG_MQTT_HEAP=64, // value of current heap
    H4P_LOG_MQTT_Q=128, // size of queue
    H4P_LOG_ALL=0xffffffff,
    H4P_LOG_ERROR=H4P_LOG_ALL
};
```

The values are chosen such that logical operations can be use , e.g. `H4P_LOG_SVC_UP | H4P_LOG_SVC_DOWN` will react only to those two service events. The default filter is `H4P_LOG_ALL`;

---

# What information gets logged?

The logging interface send the following data items to each logger:

* Msg: the content of the message
  
* Event type (see above)

* Source: Origin of the event

For `H4P_LOG_SVC_UP` and `H4P_LOG_SVC_DOWN` this will be "H4"
For `H4P_LOG_CMD` it is the sub-system that initiated the cmd
    * "serl" when issued from the serial console
    * "mqtt" in response to a subscribed topic
    * "asws" if it came from the AsyncWebServer HTTP REST interface
    * "upnp" when from a UPNP device e.g. Amazon Alexa
    * "user" when called directly from user code

* Target: will usually be empty or "self" when sent by anything other than MQTT. For subscribed topics, it will be the prefix of the MQTT publish as described in [H4P_MQTT](h4mqtt.md). In summary, one of:
    
    * all
    * < your board type > e.g. "WEMOS_D1MINI"
    * < the device chip ID > e.g. "17D858"
    * < your device name > e.g. "myIOTdevice"

---

# Loggers

*N.B.* All loggers offer a minimum command set: in the following, `xxxx` should be replaced by the name of the logger

* h4/xxxx/restart
* h4/xxxx/start
* h4/xxxx/stop

Most also implement h4/xxxx/msg/any old message to put any message you choose into the log

# H4P_SerialLogger (name "slog")

## Usage

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS
H4P_SerialLogger h4ll(...
```
## Prequisites

none

## Additional Commnds

none

## Unloadable

NO

---

## API

Constructor takes `uint32_t` filter parameter that defaults to `H4P_LOG_ALL`.

[Example Code](../examples/H4P_Loggers/H4P_Loggers.ino)

---

# H4P_LocalLogger (name "log") [ ESP8266 / ESP32 only ]

H4P_LocalLogger reserves a user-defind amount of free SPIFFS space to create a log file. When that amount has been exceeded, it will `flush` the log, i.e. it will print the contents to the serial monitor and empty the file.

**WARNING** *Choose a small value for the log file size - if the value is close to the amount of free heap (about 30k) then the MCU will crash when it tries to flush the file, as it will need more than the available RAM to read the file. This is a short-term debugging aid, **NOT** a long-term archival solution. For the latter, you need to send the log messages "offsite" to e.g. a remote server with MQTT or HTTP.*

## Usage

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS
H4P_LocalLogger h4sl;
```

## Prequisistes

Board must be compiled with an option to reserve an amount of SPIFFS

## Additional Commnds

* h4/show/log
* h4/log/clear
* h4/log/flush

## Unloadable

NO

---

# Example of raw log file

## Format

millis(),type,source,target,error,message

```cpp
6136,2,mqtt,all,0,h4/mqtt/online/flogger
52947,2,kybd,self,0,h4/reboot
101,0,h4,,0,log
106,0,h4,,0,slog
142,3,code,self,0,Xtest
146,3,code,self,0,test1
151,3,code,self,0,Ztest2
530,0,h4,,0,mqtt
648,0,h4,,0,upnp
653,0,h4,,0,asws
658,0,h4,,0,wifi
5535,2,mqtt,all,0,h4/mqtt/online/flogger
92,0,h4,,0,log
98,0,h4,,0,slog
103,3,code,self,0,normal call
108,3,code,self,0,test1
112,3,code,self,0,Ztest2
12446,2,kybd,self,0,help
```

# API

```cpp
// constructor
H4P_LocalLogger(uint32_t limit=10000,uint32_t filter=H4P_LOG_ALL); // limit=amount of free SPIFFS space to use

void clear(); // empties the log
void flush(); // show then clear
void show();

```

[Example Code](../examples/H4P_Loggers/H4P_Loggers.ino)

----
# H4P_MQTTLogger (name: see text) [ ESP8266 / ESP32 only ]

H4P_MQTTLogger differs from all other plugins, as it allows multiple instances in the same sketch - each differentiated byt the topic is publishes. The topic is alos use for the name in any command handling.

## Usage

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS
...
H4P_MQTTLogger h4m1("first",...
// optional: H4P_MQTTLogger h4m2("2nd",...
// ...               "
// optional: H4P_MQTTLogger h4m999("police",...

```

## Prequisistes

H4P_WiFi
H4P_MQTT

## Additional Commnds

none

## Unloadable

NO, but can use stop and start cmds

## API

```cpp
H4P_MQTTLogger(const string& topic,uint32_t filter=H4P_LOG_ALL);
```

[Example Code](../examples/H4P_MQTTLogger/H4P_MQTTLogger.ino)

----

# H4P_MQTTHeapLogger (name "heap") [ ESP8266 / ESP32 only ]

H4P_MQTTHeapLogger is a specalised version of H4P_MQTTLogger which periodically publishes the `heap` topic with a payload of the current free heap

## Usage

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS
...
H4P_MQTTHeapLogger h4hl(...
```

## Prequisistes

H4P_WiFi
H4P_MQTT

## Additional Commnds

none

## Unloadable

NO, but can use stop and start cmds

# API

```cpp
// constructor
H4P_MQTTHeapLogger(uint32_t frequency,uint32_t filter=H4P_LOG_ALL); // frequency is in milliseconds
```

[Example Code](../examples/H4P_MQTTHeapLogger/H4P_MQTTHeapLogger.ino)
----

# H4P_MQTTQueueLogger (name "qlog") [ ESP8266 / ESP32 only ]

H4P_MQTTHeapLogger is a specalised version of H4P_MQTTLogger which periodically publishes the `qlog` topic with a payload of H4 queue size

## Usage

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS
...
H4P_MQTTQueueLogger h4hl(...
```

## Prequisistes

H4P_WiFi
H4P_MQTT

## Additional Commnds

none

## Unloadable

NO, but can use stop and start cmds

# API

```cpp
// constructor
H4P_MQTTQueueLogger(uint32_t frequency,uint32_t filter=H4P_LOG_ALL); // frequency is in milliseconds
```

[Example Code](../examples/H4P_MQTTHeapLogger/H4P_MQTTHeapLogger.ino)

----

# Advanced topics

## Writing your own logger

Let's assume you want a logger that prints the milliseconds uptime of the MCU before each message

The code would look like this:

```cpp

#include <H4PCommon.h>

class myLogger: public H4PLogService {
        void _logEvent(const string &msg,H4P_LOG_TYPE type,const string& source,const string& target,uint32_t error){
            Serial.print("myLogger ");
            Serial.print(millis());
            Serial.print(" ");
            Serial.println(msg.c_str()); // or  Serial.println(CSTR(msg));
        }
    public:
        myLogger(): H4PLogService("mylog"){}
};

```

...and that's it! Then add the new logger to your sketch - Everything else is automatic.

[Example Code](../examples/H4P_CustomLogger/H4P_CustomLogger.ino)

---

(c) 2020 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Blog](https://8266iot.blogspot.com)
* [Facebook Esparto Support / Discussion](https://www.facebook.com/groups/esparto8266/)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook IOT with ESP8266 (moderator)}](https://www.facebook.com/groups/1591467384241011/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)
