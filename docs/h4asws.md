![H4P Flyer](/assets/HTTPLogo.jpg) 

# Asynchronous Web Server (short name="asws")
## Adds Asynchronous Webserver, AP mode configuration to H4 Universal Scheduler/Timer. Runs on ESP8266/32 only

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---

# What does it do?

Asynchronous Web Server provide bot an AP mode server to allow first-time / factory reset configuration, but also an STA mode server that provides a REST-like command interface.


## AP Mode

![Mobile AP](/assets/mobile.jpg) 

Fill in the details, click "connect" and the device will boot into STA mode using the new credentials for the rest of its days until you force a "factory reset". 

N.B. The "name" field is only relevant if you are also using the [H4P_UPNPSwitch](/h4upnp.md) plugin, when it will become the "friendly name" of the device as shown in Windows Network Explorer. Is is also the name by which Amazon Alexa will control it via voice commands "switch on < name >" and "switch off < name >"

---

# Usage

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS
H4P_WiFi h4asws(...
```

## Dependencies


* [H4P_WiFi](h4wifi.md) Plugin

* You must copy the `data` sub-folder to your sketch folder and upload to SPIFSS. To do this you will need to intall either the [ESP8266 sketch data uploader](https://github.com/esp8266/arduino-esp8266fs-plugin) or the [ESP32 sketch data uploader](https://github.com/me-no-dev/arduino-esp32fs-plugin) (or both) depending on which platform you compile for. 


## Commands Added

None, but provides HTTP REST interface with JSON replies

## Callbacks

```cpp
void onConnect(void); // webserver is up
void onDisconnect(void); // webserver is down
void h4AddAwsHandlers(void) // called after adding its own handlers, adds your in here
```

## Unloadable

No, but can be stopped with `h4/asws/stop` command.

---

# API

H4P_AsyncWebserver is a "wrapper" around the [ESPAsyncWebServer](https://github.com/philbowles/ESPAsyncWebServer) library and therefore any funtions offered by that library can be called on `h4asws.` for example `h4.asws.on(...)` to add your own handler inside the `h4AddAwsHandlers` callback.

Do not register a handler for any of the following paths:

* / 
* /rest
* /upnp

As those are use by the plugin itself

```cpp
// Constructor
H4P_AsyncWebServer(string admin="admin",string passwd="admin",H4_FN_VOID onConnect=[](){},H4_FN_VOID onDisconnect=[](){}):
// admin is the authentication user name for browsers requireing the REST interfcae
// passwd is the authentication password for browsers requireing the REST interfcae
// onConnect = user callback when webserver is up
// onDisconnect = user callback when webserver is down

// Command and control
void start();
void stop();
```

[Example Code](../examples/H4P_SONOFF_Basic/H4P_SONOFF_Basic.ino)

---

# The REST interface

The REST interface allows the user to enter commands in a similar fashion to the serial commans line or MQTT. The command to be executed is prefixed by`http://< device IP address / name >/rest/` for example `http://192.168.1.104/rest/h4/show/config`. The responsse is a JSON object.

![JSONREST](../assets/rest.jpg)

When "prettified" it looks like this:

```json

{ 
   "res":0,
   "msg":"",
   "lines":[ 
      "user=",
      "gs=Get",
      "auser=admin",
      "h4pv=0.0.1",
      "state=0",
      "udn=Socket-1_0-upnp17D848",
      "muser=",
      "target=self",
      "port=1883",
      "broker=192.168.1.4",
      "device=h4plugins",
      "ssid=XXXXXXX",
      "psk=XXXXXXXX",
      "usn=uuid:Socket-1_0-upnp17D848::Belkin:service:basicevent:1",
      "mpasswd=",
      "h4v=0.4.0",
      "chip=17D848",
      "apasswd=admin",
      "ip=192.168.1.104",
      "board=WEMOS_D1MINI",
      "name=Demo Switch",
      "source=asws"
   ]
}
```

* res is the response code

```cpp
{0,"OK"},
{1,"Unknown cmd"},
{2,"Too few parameters"},
{3,"Too many parameters"},
{4,"Numeric value expected"},
{5,"Value out of range"},
{6,"Name not known"},
{7,"Incorrect Payload Format"},
{8,"Prohibited from here"}
```

* msg is the corresponding error message if [H4P_CmdErrors](h4ce.md) is used
*  lines is an array of the same output that would have been sent a line at a time to the serial console

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
