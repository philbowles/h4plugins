![H4P Logo](/assets/DiagLogo.jpg)

# Logging

## Adds logging to H4 Universal Scheduler/Timer + Plugins.

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---

# What do they do?

H4Plugins loggers can send cmd input (and result code) to a variety of destinations, depending on which logger you choose. There are currently 

* H4P_LocalLogger which creates a log file in SPIFFS thus requires you allocating an amount at compile-time with Tools... menu
* H4P_SerialLogger which does exactly what it says on the tin

"In the pipeline" are an HTTP REST logger, MQTT logger and MySQL logger. Writing your own is seriously easy (see below)

The important thing to note is that each logger is called in turn, thus you can log to several destinations at a time for a single message. The serial logger is a useful diagnostic aid, as it shows what is getting logged to other - less visible - destinations, e.g. remote servers.

Also note that the main interface `h4sc.logEvent("some message")` will simply do nothing if no loggers are installed. This allows it to be left in the code and "switched on or off" by commenting out the loggers, which can be flipped back in at a stroke for testing.

Better still. there is a macro EVENT("some message) which simply calls `h4sc.logEvent` but can be "compiled out" by removing the `#define H4P_LOG_EVENTS` entry in `H4PConfig.h`, then there is zero overhead.

---

# What gets logged?

The logger interface send the following data items to each logger:

```cpp
void _logEvent(const string &msg,H4P_LOG_TYPE type,const string& source,const string& target,uint32_t e){;

```
* Msg: the content of the message
  
* Message type, one of:

```cpp
enum H4P_LOG_TYPE {
    H4P_LOG_SVC_UP, // internal H4 event on service UP
    H4P_LOG_SVC_DOWN, // internal H4 event on service DOWN
    H4P_LOG_CMD, // command from any source (see below)
    H4P_LOG_USER // arbitraty message from user code
};
```
* Source: Origin of the event

For `H4P_LOG_SVC_UP` and `H4P_LOG_SVC_DOWN` this will be "H4"
For `H4P_LOG_CMD` it is the sub-system that initiated the cmd
    * "serl" when issued from the serial console
    * "mqtt" in response to a subscribed topic
    * "asws" if it came from the AsyncWebServer HTTP REST interface
    * "user" when called directly from user code

* Target: will usually be empty or "self" when sent by anything other than MQTT. For subscribed topics, it will be the prefix of the MQTT publish as described in [H4P_MQTT](h4mqtt.md). In summary, one of:
    
    * all
    * < your board type > e.g. "WEMOS_D1MINI"
    * < the device chip ID > e.g. "17D858"
    * < your device name > e.g. "myIOTdevice"

* Error Code: return value if Type = `H4P_LOG_CMD`, otherwise zero.
  
---

# Loggers

*N.B.* All loggers offer a minimum command set: in the following, `xxxx` should be replaced by the name of the logger

* h4/xxxx/msg/any old message // send "any old message" to the log
* h4/xxxx/restart
* h4/xxxx/start
* h4/xxxx/stop

They all also support `restart`,`start` and `stop` API functions as with all other H4Plugins services

# H4P_SerialLogger (name "slog")

## Usage

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS
H4P_SerialLogger h4sl;
```
## Prequisites

none

## Additional Commnds

none

## Trusted Name

*SLOG*

## Unloadable

NO

---

## API

none

[Example Code](../examples/H4P_Loggers/H4P_Loggers.ino)

---

# H4P_LocalLogger (name "log") [ ESP8266 / ESP32 only ]

H4P_LocalLogger reserves a user-defind amount of free SPIFFS space to create a log file. When that amount has been exceeded, it will `flush` the log, i.e. it will print the contents to the serial monitor and empty the file.

**WARNING** *Choose a small value for the log file size - if the value is close to the amount of free heap (about 30k) then the MCU will crash when it tries to flush the file, as it will need more than the available RAM to read the file. This is a short-term debugging aid, **NOT** a long-term archival solution. For the latter, you need to send the log messages "offsite" to e.g. a remote server with MQTT or HTTP.*

## Usage

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS
H4P_SerialLogger h4sl;
```

## Prequisistes

Board must be compiled with an option to reserve an amount of SPIFFS

## Additional Commnds

* h4/show/log
* h4/log/clear
* h4/log/flush

## Trusted Name

*LOG*

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
H4P_LocalLogger(uint32_t limit=10000); // amount of free SPIFFS space to use

void clear(); // empties the log
void flush(); // show then clear
void show();

```

[Example Code](../examples/H4P_Loggers/H4P_Loggers.ino)

----

# Advanced topics

## Writing your own logger

Let's assume you want a logger that prints the milliseconds uptime of the MCU before each message

The code would look like this:

```cpp

#include <H4PCommon.h>

class myLogger: public H4PLogService {
        void        _logEvent(const string &msg,H4P_LOG_TYPE type,const string& source,const string& target,uint32_t error){
            if(_running) {
                Serial.print("myLogger ");
                Serial.print(millis());
                Serial.print(" ");
                Serial.println(msg.c_str()); // or  Serial.println(CSTR(msg));
            }
        }
    public:
        myLogger(): H4PLogService("mylog"){
            subid=H4PC_MYLOG; // see note below
            _names={ {H4P_TRID_MYLOG,uppercase(_pid)} }; // see note below
        }
};

```

**Note** to get the next values for `H4PC_MYLOG` and `H4P_TRID_MYLOG` you need to edit `H4PCommon.h`

```cpp
...
  H4P_TRID_NTFY,
  H4P_TRID_UBSW,
  H4P_TRID_3FNB,
  H4P_TRID_CERR,
  H4P_TRID_SCMD,
  H4P_TRID_QWRN,
  H4P_TRID_SNIF,
  H4P_TRID_LLOG,
  H4P_TRID_SLOG,
  H4P_TRID_CURL, // comma added by you
  H4P_TRID_MYLOG // ADD THIS HERE and put a comma at the end of the line above
};

enum H4PC_CMD_ID{
    H4PC_ROOT=1,
    H4PC_SHOW,
    subid,
    H4PC_QWRN,
    subid,
    H4PC_ESW_SET,
    H4PC_ESW_SWEEP,
    H4PC_WIFI,
    H4PC_MQTT,
    H4PC_ASWS,
    H4PC_SPIF,
    H4PC_UPNP, 
    H4PC_LLOG, 
    H4PC_SLOG,
    H4PC_CURL, // comma added by you
    H4PC_MYLOG // ADD THIS HERE and put a comma at the end of the line above
};
...
```

...and that's it! Then add the new logger to your sketch - Everything else is automatic.

[Example Code](../examples/H4P_CustomLogger/H4P_CustomLogger.ino)

## Output form Example sketch
```cpp
slog
SVC slog UP
mylog
myLogger 117 mylog
SVC mylog UP
normal call
myLogger 117 normal call
test1
myLogger 117 test1
Ztest2
myLogger 118 Ztest2
scmd: h4/dump
scmd: h4/mylog/msg
scmd: h4/mylog/restart
scmd: h4/mylog/start
scmd: h4/mylog/stop
scmd: h4/reboot
scmd: h4/show/all
scmd: h4/show/config
scmd: h4/show/q
scmd: h4/show/qstats
scmd: h4/show/spif
scmd: h4/show/tnames
scmd: h4/show/unload
scmd: h4/slog/msg
scmd: h4/slog/restart
scmd: h4/slog/start
scmd: h4/slog/stop
scmd: h4/unload
scmd: help
help
myLogger 40943 help

```

---

## MySQL schema (for future use)

CREATE TABLE `event` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `type` int(11) NOT NULL,
  `error` int(11) NOT NULL,
  `source` varchar(16) DEFAULT NULL,
  `target` varchar(16) DEFAULT NULL,
  `msg` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id_UNIQUE` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci

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
