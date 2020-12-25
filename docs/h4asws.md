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
H4P_WiFi h4wifi(...
H4P_AsyncWebServer h4asws(...
```

## Dependencies

* [H4P_WiFi](h4wifi.md) Plugin

* You must copy the `data` sub-folder to your sketch folder and upload to SPIFSS. To do this you will need to intall either the [LittleFS upload tool](https://github.com/earlephilhower/arduino-esp8266littlefs-plugin) or the [ESP32 sketch data uploader](https://github.com/me-no-dev/arduino-esp32fs-plugin) (or both) depending on which platform you compile for. 


## Commands Added

* `h4/asws/msg/X` (payload X = anything: gets sent to message area of webui)
* `h4/asws/gpio/pin#|all` (payload either "all" or a valid GPIO pin number: adds user GPIO LED
* `h4/asws/uichg/name,value` (used internally: not normally called by user: set user input item "name" to value
  
---

# User-defined fields

Since v0.5.6 user can add their own data fields to the main UI.

## Initial Creation

In `h4Setup` the user creates his/her named UI fields which can be any of:

* Text Labels
* Numeric Labels
* Boolean red/green LED (optionally clickable)
* Input
* Dropdown List

Boolean fields can also be clickable to send messages back from the UI to switch on/off, start/stop processes. If a clickable Boolean is required, the user must additionally provide a callback "action" function to handle/react to the on/off click). Other updateable fields (input, dropdown) call a single callback `onUiChange`

User may choose to provide single fixed text / numeric / boolean value when field is created - this is best for single static values that will not change.

For more volatile data that may have changed by the time any viewers load the UI, he/she may instead or provide a named function which will provide the appropriate value when the webUI is actually loaded.

## Dynamic updates

Any field can be set asynchronously to any arbitrary value, by calling one of the `uiSet...` functions. Fields which were create originally with a "setter" function can force the setter function to be called - thereby refreshing the field with its latest current value - by calling `uiSync()` (which synchronises *all* such fields in the one call).

---

# API

## Callbacks

```cpp
// general
void onViewers(void); // User requests web page: add user items + "setter" functions
void onNoViewers(void); // no viewers: cancel timers, clean up resources etc
void onUiChange(const string& name,const string& ref value); // called when user item <name> changes value
```

H4P_AsyncWebserver is a "wrapper" around the [ESPAsyncWebServer](https://github.com/philbowles/ESPAsyncWebServer) library and therefore any functions offered by that library can be called on `h4asws.` for example `h4.asws.on(...)` to add your own handler.

Do not register a handler for any of the following paths:

* / 
* /rest
* /upnp
* /uichg

As those are use by the plugin itself

```cpp
// Constructor
// onViewers callback when first user requests web page: add user items + "setter" functions
// onNoViewers  callback when last viewer closes browser: cancel timers, clean up resources etc
H4P_AsyncWebServer(H4_FN_VOID onViewers=nullptr,H4_FN_VOID onNoViewers=nullptr):
//
// name =name of your own UI field
// setter = name of "setter" function which returns data of the appropriate type to popultae the field when web page requested
// action = name of "action" function which is called when value changes. If defulted, field is not user-clickable
// 
void uiMessage(const string& msg); // msg scrolls in red at bottom of screen 
void uiAddBoolean(const string& name,const boolean truefalse,H4_FN_UIACTIVE action=nullptr); // create static boolean LED
void uiAddBoolean(const string& name,H4_FN_UIBOOL setter,H4_FN_UIACTIVE action=nullptr); // dynamic bool set on demand by setter function
void uiAddDropdown(const string& name,H4P_CONFIG_BLOCK options); // create dropdown list of OPTION/VALUE pairs (see onUiChange callback)
void uiAddGPIO(); // add output-only boolean LED for every managed GPIO pin
H4_CMD_ERROR uiAddGPIO(uint8_t pin); // add output-only boolean LED tied to GPIO pin. Returns H4_CMD_OUT_OF_BOUNDS if not a valid pin
void uiAddInput(const string& name,H4_FN_UITXT setter=nullptr); // add input field with optional intial value dynamically set (see onUiChange callback)
void uiAddLabel(const string& name,const int value); // Create static numeric label 
void uiAddLabel(const string& name,H4_FN_UINUM setter);// Create dynamic numeric label set on demand by setter function
void uiAddLabel(const string& name,const string& value); // Create static text label 
void uiAddLabel(const string& name,H4_FN_UITXT setter)// Create dynamic text label set on demand by setter function
void uiMessage(const string& format, Args... args); // works like printf to send message to scrolling alert area of UI in red
void uiSetInput(const string& name,const string& value); // arbitrarily set input field to value
void uiSetBoolean(const string& name,const bool truefalse);// arbitrarily set boolean to true or false
void uiSetLabel(const string& name,const int value);// arbitrarily set numeric value
void uiSetLabel(const string& name,const string& value);// arbitrarily set text field to value
void uiSync(); // force all setters to update fields to current value

```

[Example Code - static fields](../examples/WEBUI/WebUI_StaticFields/WebUI_StaticFields.ino)
[Example Code - dynamic fields](../examples/WEBUI/WebUI_DynamicFields/WebUI_DynamicFields.ino)
[Example Code - input fields](../examples/WEBUI/WebUI_InputFields/WebUI_InputFields.ino)

---

# The REST interface

The REST interface allows the user to enter commands in a similar fashion to the serial command line or MQTT. The command to be executed is prefixed by`http://< device IP address or name >/rest/` for example `http://192.168.1.104/rest/h4/show/config`. The response is a JSON object.

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
