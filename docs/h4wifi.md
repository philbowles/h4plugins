![H4P Flyer](../assets/WiFiLogo.jpg) 

# H4P_WiFi

## Service shortname wifi

Provides WiFi management / reconnection, Asynchronous Webserver, and OTA updates.

---

## Contents

* [Usage](#usage)
* [Dependencies](#dependencies)
* [Commands Added](#commands-added)
* [Service Commands](#service-commands)
* [API](#api)

---
# Usage

```cpp
H4P_WiFi whiffy(...
```

This plugin is a "singleton" - there may be only one single instance of it in the app. 
It may be instantiated as any name the user chooses, prefix all API calls below with that name.

# Dependencies

[H4P_PinMachine](h4gm.md)
[H4P_Signaller](h4gm.md)

* You must copy the `data` sub-folder to your sketch folder and upload to LittleFS (ESP8266) or SPIFFS (ESP32). To do this you will need to intall either the [LittleFS upload tool](https://github.com/earlephilhower/arduino-esp8266littlefs-plugin) or the [ESP32 sketch data uploader](https://github.com/me-no-dev/arduino-esp32fs-plugin) (or both).
  
* On devices with > 1MB flash also copy the images in the `data_jpg` folder for an enhanced UI experience

# Commands Added

* `h4/wifi/change/x,y` (payload x,y = newssid,newpassword)
* `h4/wifi/msg/x` (payload x = messge to be sent to scrolling message area in webUI

# Service Commands

`stop` will disconnect from WiFi and initiate closedown of all Plugins the depend on WiFi
`start` will connect to WiFi and start webserver and all dependent services

---

## API

For guidance on the use of the `ui...` functions, see [Customising the web UI](webui.md)

```cpp
/*
Constructor
ssid / psk = your router credentials. psk = pre-shared key = password
device is the local name for e.g. OTA. Will be visible in your system as < device >.local
*/
H4P_WiFi(string ssid,string psk,string device=""):

// Command and control
void change(string ssid,string psk); // connect to new SSID
/*
webUI functions

Common parameters:

const string& name; Title Of The Field...It Gets "Proper Cased" "oh my" becomes "Oh My" 
const string& section="u"; the webUI panel to add the field to.
It is recommended to leave this as the default "u" for User fields.

*/
uiAddBoolean(const string& name,const string& section="u"); // fixed red/green boolean as tf=true or false
uiAddDropdown(const string& name,H4P_NVP_MAP options,const string& section="u");// dropdown box from maps of options
uiAddGlobal(const string& name,const string& section="u"); // Take field values from h4p[name]
uiAddImg(const string& name,const string& url,const string& section="u");// adds image from url
uiAddImgButton(const string& name,const string& section="u");// adds clickable image (simulates a button)
uiAddInput(const string& name,const string& section="u")// simple text input
uiAddText(const string& name,const string& v,const string& section="u")// fixed string value
uiAddText(const string& name,int v,const string& section="u")// fixed int value
uiAddAllUsrFields(const string& section="u");// searches for all global variable starting usr_... and adds them to UI
uiMessage(const string& msg, Args... args); // Show scrolling printf-style message in UI
uiSetValue(const string& ui,const int f); // change int field value
uiSetValue(const string& ui,const string& value); // change string field value
```

---

# Example sketches

* [Web UI Static Fields](../examples/06_WEB_UI/WebUI_StaticFields/WebUI_StaticFields.ino)
* [Web UI Input Fields](../examples/06_WEB_UI/WebUI_InputFields/WebUI_InputFields.ino)
* [Add remote data](../examples/06_WEB_UI/WebUI_RemoteInclusions/WebUI_RemoteInclusions.ino)
  
---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)