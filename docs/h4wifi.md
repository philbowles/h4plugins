![H4P Flyer](/assets/WiFiLogo.jpg) 

# H4P_WiFi

## Shortname wifi

Provides WiFi management / reconnection, Asynchronous Webserver, AP mode configuration* and OTA updates.

---

## Contents

* [Usage](#usage)
* [Dependencies](#dependencies)
* [Commands Added](#commands-added)
* [Events Emitted](#s-emitted)
* [Events Listened for](#s-listened-for)
* [Tasks Run](#tasks-run)
* [Service Commands](#service-commands)
* [API](#api)

# Usage

```cpp
H4P_WiFi whiffy(...
```
This plugin is a "singleton" - there may be only one single instance of it in the app. 
It may be instantiated as any name the user chooses, prefix all API calls below with `myChosenName.`

# Dependencies

[H4P_GPIOManager](h4gm.md)
[H4P_FlasherController](h4gm.md)

* You must copy the `data` sub-folder to your sketch folder and upload to LittleFS (ESP8266) or SPIFFS (ESP32). To do this you will need to intall either the [LittleFS upload tool](https://github.com/earlephilhower/arduino-esp8266littlefs-plugin) or the [ESP32 sketch data uploader](https://github.com/me-no-dev/arduino-esp32fs-plugin) (or both). 

### Commands Added

* h4/factory (clear credentials + reboot)
* h4/wifi/change/x,y (payload x,y = newssid,newpassword)
* h4/wifi/clear (use with caution = "factory reset")
* h4/wifi/host/x (payload x = new device name. Causes a reboot. Remains until factory reset)

# Events Listened for

`H4P_EVENT_FACTORY`

# Events Emitted


# Tasks Run

| Symbolic Name | Short Name | Type | Singleton | Purpose |
| :----------   | :--- | :--- | :-------: | :---    |
|H4P_TRID_HOTA|OTA|every|:heavy_check_mark:|OTA handler|

# Service Commands

`stop` will disconnect from WiFi and initiate closedown of all Plugins the depend on WiFi
`start` will connect to WiFi and start webserver and all dependent services

## Callback functions

```cpp
void onConnect(void);
void onDisconnect(void);
void onNoViewers(void); // no viewers: cancel timers, clean up resources etc
void onUiChange(const string& name,const string& ref value); // called when user item <name> changes value if no spcific funtion given
void onViewers(void); // User requests web page: add user items + "setter" functions
```

---

## API

For guuidance on the sue of the `ui...` functions, see [Customising the web UI](webui.md)

```cpp
/*
Constructor
ssid / psk = your router credentials. psk = pre-shared key = password
device is the local name for e.g. OTA. Will be visible in your system as < device >.local
onConnect = user callback when WiFi connects / reconnects
onDisconnect = user callback when WiFi disconnects
*/
H4P_WiFi(string ssid,string psk,string device="",H4_FN_VOID onC=nullptr,H4_FN_VOID onD=nullptr):

// Command and control
void change(string ssid,string psk); // connect to new SSID
void clear(); // erase any previoulsy stored credentials - think: "factory reset"
void host() // change device name. Causes a reboot
/*
webUI functions

Common parameters:

const string& name; Title Of The Field...It Gets "Proper Cased" "oh my" becomes "Oh My" 

f = a delayed value getter function
using H4P_FN_UITXT      = function<string(void)>;
using H4P_FN_UINUM      = function<int(void)>;
using H4P_FN_UIBOOL     = function<boolean(void)>;

bool repeat; // false = single-shot, true = repeating

onChange = function called when user changes input field, if none specified, goes to global callback onUiChange

All of the uiAdd.. functios return the field's ID, required by:
...all of the uiSet... functions for later changes to the value
...and uiSync for specific fields.

*/
uint32_t uiAddLabel(const string& name); // special case, take value of config variable of the same name
uint32_t uiAddLabel(const string& name,const string& v); // fixed string value
uint32_t uiAddLabel(const string& name,const int v); // fixed int value
uint32_t uiAddLabel(const string& name,H4P_FN_UITXT f,bool repeat=false); // delayed getter string
uint32_t uiAddLabel(const string& name,H4P_FN_UINUM f,bool repeat=false); // delyed getter int
void uiAddGPIO(); //  Add ALL managed GPIO pins as red/green booleans (page refresh needed)
uint32_t uiAddGPIO(uint8_t pin); //  Add specific GPIO pin as red/green boolean (page refresh needed)
uint32_t uiAddBoolean(const string& name,const boolean tf); // fixed red/green boolean as tf=true or false
uint32_t uiAddBoolean(const string& name,H4P_FN_UIBOOL f,H4P_FN_UICHANGE onChange=nullptr,bool repeat=false); // clickable red/green boolean
uint32_t uiAddDropdown(const string& name,H4P_CONFIG_BLOCK options,H4P_FN_UICHANGE onChange=nullptr); // dropdown box from maps of options
uint32_t uiAddInput(const string& name,const string& value="",H4P_FN_UICHANGE onChange=nullptr); // simple text input
void uiSetInput(uint32_t ui,const string& value); // forcibly change value of input field
void uiSetBoolean(uint32_t ui,const bool b); // change value of red/green bool
void uiSetLabel(uint32_t ui,const int f); // change int field value
void uiSetLabel(uint32_t ui,const string& value); // change string field value
void uiSync(); // redisplay all fields with repeating getter function
void uiSync(uint32_t ui); // redisplay specific repeating getter
void uiMessage(const string& msg, Args... args); // Show scrolling printf-style message in UI
```

---

# Example sketches

* [Web UI Static Fields](../examples/WEBUI/WebUI_StaticFields/WebUI_StaticFields.ino)
* [Web UI Dynamic Fields](../examples/WEBUI/WebUI_DynamicFields/WebUI_DynamicFields.ino)
* [Web UI Input Fields](../examples/WEBUI/WebUI_InputFields/WebUI_InputFields.ino)
  
---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)