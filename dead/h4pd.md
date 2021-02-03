![H4P Flyer](/assets/GPIOLogo.jpg) 

# Presence Detectors

## React to devices joining / leaving network

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---

# What do they do?

Presence detectors allow the user to be notified via callback when a specific device is in range / connected to the network and again when it disconnects or goes out of range.

Reacting to IP address:

* H4P_IPDetector
* H4P_IPDetectorSource

Reacting to UPnP USN

* H4P_UPNPDetector
* H4P_UPNPDetectorSource

Reacting to another H4 device

* H4P_H4Detector
* H4P_H4DetectorSource

Reacting to an MDNS service/protocol

* H4P_H4Detector
* H4P_H4DetectorSource

Each has a "friendly name" and usually a unique device-specifc ID which is what is actually detected. When the status of the ID on the network changes, your code is called back with a `bool` set to `true` if the device is joining the network or `false` if disconnected.

Each detector also has a "Source" variant ( see ["Switches vs Sources"](things.md) ) which can be linked directly to a GPIO.

When creating the detector, its service name is the same as the friendly name, so it is best to keep them short and avoid space, punctuation etc.

## Simple Example

Assume you have a mobile phone with the name "Elvis" and fixed IP address of 192.168.1.31

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop
...
H4P_IPDetector ippdElvis("Elvis","192.168.1.31",[](bool b){ Serial.printf("Elvis has %s the building\n",b ? "entered":"left"); });
...
```

Now imagine instead that you have a [BinarySwitch](things.md) on, say, GPIO12 and this is connected to an outside security light. As you approach your house and your phone connects to your WiFi, the security light goes on.

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop
...
H4P_BinarySwitch h4onof(12,ACTIVE_HIGH,OFF); // security light
H4P_IPDetectorSource ippdElvis("Elvis","192.168.1.31"); // switch light on when in range
...
```

Finally, assume you would like your device to turn on / off when another h4 joins or leaves the network.

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop
...
H4P_BinarySwitch h4onof(12,ACTIVE_HIGH,OFF); // security light
H4P_H4Detect linked("anotherH4"); // switch light on when anotherH4 is on network
...
```

---

## Dependencies

* [H4P_WiFi](h4wifi.md) Plugin

## Commands Added

none

---

# API

```cpp
/* Constructors
friendly = human-readable name by which device is known
id = device spcific network identifier
f = callback function: yourfuntion(bool presence){}
service = MDNS service name
protocol = MDNS protocol
*/
H4P_H4Detector(const string& friendly,H4BS_FN_SWITCH f=nullptr); // friendly = h4 device name. no id needed as it IS the friendly name
H4P_H4DetectorSource(const string& friendly); // friendly = h4 device name. no id needed as it IS the friendly name
H4P_IPDetector(const string& friendly,const string& id,H4BS_FN_SWITCH f=nullptr); // id = IP address
H4P_IPDetectorSource(const string& friendly,const string& id); // id = IP address
H4P_MDNSDetector(const string& friendly,const string& service,const string& protocol,H4BS_FN_SWITCH f=nullptr);
H4P_MDNSDetectorSource(const string& friendly,const string& service,const string& protocol);
H4P_UPNPDetector(const string& friendly,const string& usn,H4BS_FN_SWITCH f=nullptr); // id = UPnP USN
H4P_UPNPDetectorSource(const string& friendly,const string& id); // id = UPnP USN

All the above provide:
bool isPresent(); // returns true/false depending on whether device is present / absent
```

# Examples

[Example Sketch - BinaryThing](../examples/THINGS/H4P_BinaryThing/H4P_BinaryThing.ino)
[Example Sketch - BinarySwitch](../examples/THINGS/H4P_BinarySwitch/H4P_BinarySwitch.ino)
[Example Sketch - BinarySwitch with 3-function button](../examples/THINGS/H4P_BinarySwitchmfnb/H4P_BinarySwitchmfnb.ino)
[Example Sketch - BinarySwitch with MQTT](../examples/THINGS/H4P_BinarySwitchMQTT/H4P_BinarySwitchMQTT.ino)
[Example Sketch - UPNPServer with MQTT](../examples/THINGS/H4P_SONOFF_Basic/H4P_SONOFF_Basic.ino)

---


(c) 2020 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Blog](https://8266iot.blogspot.com)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook IOT with ESP8266 (moderator)](https://www.facebook.com/groups/1591467384241011/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)
