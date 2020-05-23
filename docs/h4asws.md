![H4P Flyer](/assets/HTTPLogo.jpg) 

# Asynchronous Web Server (short name="asws")
## Adds Asynchronous Webserver to H4 Universal Scheduler/Timer. Runs on ESP8266/32 only

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---

# What does it do?

Asynchronous Web Server provide both an AP mode server to allow first-time / factory reset configuration and an STA mode server.
The webserver provides a REST-like command interface at `http:// your ip /rest/....` and a dynamically updating web UI with command console.

![Mobile AP](/assets/webui056.jpg) 

---

## AP Mode

![Mobile AP mode](/assets/apmode.jpg) 

Fill in the details, click "connect" and the device will boot into STA mode using the new credentials for the rest of its days until you force a "factory reset" or `h4/wifi/apmode`

N.B. The "name" field is only relevant if you are also using the [H4P_UPNPServer](/things.md) plugin, when it will become the "friendly name" of the device as shown in Windows Network Explorer. Is is also the name by which Amazon Alexa will control it via voice commands "switch on < name >" and "switch off < name >"

---

# Usage

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop
H4P_WiFi h4asws(...
```

## Dependencies

* [H4P_WiFi](h4wifi.md) Plugin

* You must copy the `data` sub-folder to your sketch folder and upload to SPIFSS. To do this you will need to intall either the [ESP8266 sketch data uploader](https://github.com/esp8266/arduino-esp8266fs-plugin) or the [ESP32 sketch data uploader](https://github.com/me-no-dev/arduino-esp32fs-plugin) (or both) depending on which platform you compile for. 


## Commands Added

* `h4/asws/msg/X` (payload X = anything: gets sent to message area of webui)
  
---

# User-defined fields

Since v0.5.6 user can add their own data fields to the main UI. The user must provide a "setter" function for each field which returns the value to be displayed. Once the web UI is active, i.e. there is at least one browser / viewer, the user is also able to asynchronously update and of his/her fields. 
Currently available are:

* Text Labels (from a user `string`-returning function )
* Numeric Labels (from a user `int`-returning function )
* Boolean red/green LED (from a user `bool`-returning function )

Boolean fields can also be clickable to send messages back from the UI to switch on/off, start/stop processes. If a clickable Boolena is required, the user must additionally provide a callback "action" function to handle/react to the on/off click

# API

## Callbacks

```cpp
// general
void onFirstClient(void); // User requests web page: add user items + "setter" functions
void onLastClient(void); // no viewers: cancel timers, clean up resources etc
// ui "setter"s
string fUserStringSetter(void); // user function that returns data for a LabelText UI field
int fUserIntSetter(void); // user function that returns data for a LabelNumeric UI field
string fUserBoolSetter(void); // user function that returns data for a Boolean UI field
// "action" funtion for clickable boolean
void onUIBoolClick(string name,string value); // name is the user's name for the UI field, value will be "1" or "0"

```

H4P_AsyncWebserver is a "wrapper" around the [ESPAsyncWebServer](https://github.com/philbowles/ESPAsyncWebServer) library and therefore any funtions offered by that library can be called on `h4asws.` for example `h4.asws.on(...)` to add your own handler.

Do not register a handler for any of the following paths:

* / 
* /rest
* /upnp

As those are use by the plugin itself

```cpp
// Constructor
// onFirstClient callback when first user requests web page: add user items + "setter" functions
// onLastClient  callback when last viewer close browser: cancel timers, clean up resources etc
H4P_AsyncWebServer(H4_FN_VOID onFirstClient=nullptr,H4_FN_VOID onLastClients=nullptr):
//
// name =name of your own UI field
// f... = name of "setter" function which returns data of the appropriate type to popultae the field when web page requested
void addUILabelNumeric(const string& name,H4_FN_UINUM fUserIntSetter);
void addUILabelText(const string& name,H4_FN_UITXT fUserStringSetter);
void addUIBoolean(const string& name,H4_FN_UIBOOL fUserBoolSetter);
// change value of exisitng user webUI field
void setUILabelNumeric(const string& name,H4_FN_UINUM fUserIntSetter);
void setUILabelText(const string& name,H4_FN_UITXT fUserStringSetter);
void setUIBoolean(const string& name,H4_FN_UIBOOL fUserBoolSetter);
//
void sendUIMessage(const string& msg); // msg scrolls in red at bottom of screen 

```

[Example Code - static fields](../examples/WEBUI/WebUI_StaticFields/WebUI_StaticFields.ino)
[Example Code - dynamic fields](../examples/WEBUI/WebUI_DynamicFields/WebUI_DynamicFields.ino)

---

# The REST interface

The REST interface allows the user to enter commands in a similar fashion to the serial commans line or MQTT. The command to be executed is prefixed by`http://< device IP address or name >/rest/` for example `http://192.168.1.104/rest/h4/show/config`. The response is a JSON object.

![JSONREST](../assets/rest.jpg)

When "prettified" it looks like this:

```json

{ 
   "res":0,
   "msg":"",
   "lines":[ 
      "user=",
      "gs=Get",
      "h4pv=0.5.4",
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
      "ip=192.168.1.104",
      "board=WEMOS_D1MINI",
      "name=Demo Switch",
      "source=asws"
   ]
}
```

* `res` is the response code [List](h4ce.md)
* `msg` is the corresponding error message if [H4P_CmdErrors](h4ce.md) is used
* `lines` is an array of the same output that would have been sent a line at a time to the serial console

---

(c) 2020 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Blog](https://8266iot.blogspot.com)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook IOT with ESP8266 (moderator)}](https://www.facebook.com/groups/1591467384241011/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)
