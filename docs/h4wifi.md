![H4P Flyer](/assets/WiFiLogo.jpg) 

# WiFi Manager

## Adds WiFi management / reconnection, Asynchronous Webserver, AP mode configuration and OTA updates to H4 Universal Scheduler/Timer. Runs on ESP8266/32 only

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---

# What does it do?

H4 WiFi Manager controls all aspects of WiFi connection and reconnection after failure. It allows the main body of your code to keep on running if WiFi fails and will automatically re-connect "in the background" when WiFi becomes available again. It also allows seamless change of connection to a different SSID and provides the ability to update your code "Over The Air" (OTA).

![upnp](/assets/upnp.jpg) 

---

# Usage

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop
H4P_WiFi h4wifi(...
```

### Dependencies

* You must copy the `data` sub-folder to your sketch folder and upload to SPIFFS. To do this you will need to intall either the [ESP8266 sketch data uploader](https://github.com/esp8266/arduino-esp8266fs-plugin) or the [ESP32 sketch data uploader](https://github.com/me-no-dev/arduino-esp32fs-plugin) (or both) depending on which platform you compile for. 


### Commands Added

* h4/factory (clear credentials + reboot: forces AP mode)
* h4/wifi/apmode (factoryReset, force AP mode)
* h4/wifi/change/x,y (payload x,y = newssid,newpassword)
* h4/wifi/clear (use with caution = "factory reset")
* h4/wifi/host/x (payload x = new device name. Causes a reboot. Remains until factory reset)

### Callbacks

```cpp
void onConnect(void);
void onDisconnect(void);
```

---

## API

```cpp
// Constructor
H4P_WiFi(string ssid,string psk,string device="",H4_FN_VOID onConnect=[](){},H4_FN_VOID onDisconnect=[](){});
// device is the local name for e.g. OTA. Will be visible in your system as < device >.local
// onConnect = user callback when WiFi connects / reconnects
// onDisconnect = user callback when WiFi disconnects

// Command and control
void change(string ssid,string psk); // connect to new SSID
void forceAP(); // performs factoryReset and boots into AP Mode
void clear(); // erase any previoulsy stored credentials - think: "factory reset"
void host() // change device name. Causes a reboot
```

### Device naming

If no device name is given in the constructor, it defaults to H4_XXXXXX where XXXXXX is the unique chip ID of the device (usually the last 6 characters of the MAC address).

This is useful to enable a single generic sketch to be uploaded to numerous devices without change. See [Advanced Topics](advanced.md)

### "Tweakables"

The following values are defined in `config.h` . They are chosen initally to set a good balance between stability, performance and memory / stack usage. *It is not advisable to change them unless you know exactly what you are doing and why*. 

**N.B.** **Support will not be provided if any of these values are changed.**

* H4WF_OTA_RATE

The number of milliseconds for each call of `ArduinoOTA,handle()`. Most basic sketches call this on every loop, which amounts to 40000 - 60000 times *per second* which is a massive peformance hit and totally unnecessary.

The larger the value, the less of a performace hit, BUT the longer it takes for any other software to recognise this device as OTA-able. If you increase this value, you may go past the point where e.g. ArduinoIDE can "see" it at all. If you *decrease* it. the only thing that will happen is your overall performance will be hurt.

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
