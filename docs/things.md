![H4P Flyer](/assets/GPIOLogo.jpg) 

# BinarySwitch and BinarySource (shortname="onof")

## Link on/off/toggle commands to a GPIO or functional object for H4 Universal Scheduler/Timer.

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---

# What do they do?

Let's not forget that "IOT" is the Internet of *Sources*. So what is a "Source"? The answer is: *anything you want it to be*. In H4 Plugins terms, it is an object that holds an interal `state` of `ON` or `OFF` and reacts to commands `on`,`off`,`toggle` and `switch`. If you are using the  [H4P_AsyncMQTT plugin](h4mqtt.md) it publishes the `state` topic whenever the state changes. A Switch is a just a Source with a predefined output function linked to a single GPIO.

There can only be one *output* Source (or Switch) in a sketch/app and it must be named as `h4onof`. It is effectively the default handler for the above commands. It is what your device does when any source ( [Serial](h4cmd.md), [MQTT](h4mqtt.md), [HTTP REST](h4asws.md), Amazon Alexa voice command or [linked GPIO input connector](h5gpio.md) ) sends an `on`,`off`,`toggle` or `switch` command.

You can have many *input* [linked GPIO input connector](h5gpio.md)s all linked to the single *output* `h4onof`. Each of these behaves exactly like its non-Source counterpart except that instead of calling back your code when its state changes, it automatically sends its ON or OFF state to the `h4onof` handler, either a BinarySwitch or a BinarySource.

* AnalogThresholdSource [Example Code](../examples/H4GM_AnalogThresholdSource/H4GM_AnalogThresholdSource.ino)
* DebouncedSource [Example Code](../examples/H4GM_DebouncedSource/H4GM_DebouncedSource.ino)
* EncoderSource [Example Code](../examples/H4GM_EncoderSource/H4GM_EncoderSource.ino)
* LatchingSource [Example Code](../examples/H4GM_LatchingSource/H4GM_LatchingSource.ino)
* PolledSource [Example Code](../examples/H4GM_PolledSource/H4GM_PolledSource.ino)
* RawSource [Example Code](../examples/H4GM_RawSource/H4GM_RawSource.ino)
* RetriggeringSource [Example Code](../examples/H4GM_RetriggeringSource/H4GM_RetriggeringSource.ino)

In addition, you may also have:
* [H4P_MultiFunctionButton](h4mfnb.md) [Example Code](../examples/H4P_SONOFF_Basic/H4P_SONOFF_Basic.ino)
* H4P_UPNPServer [Example Code](../examples/H4P_SONOFF_Basic/H4P_SONOFF_Basic.ino)

The [H4P_MultiFunctionButton](h4mfnb.md) provides an easy way to switch your Source/Switch with a physical button, reboot it or factory reset it, depending on how long you hold it down.

`H4P_UPNPServer` adds Amazon Alexa voice control and Windows10 desktop integration. It requires only a "friendly name" e.g. "Kitchen Light" - this is the name by which Alexa will turn it on or off and the name show on the Windows10 desktop.

## Simple Example

Assume you have an output GPIO on pin 12 wired to a relay that controls a security light

Now add some inputs:

* Pushbutton on GPIO0
* PIR sensor on GPIO4
* Magnetic reed switches on front door (GPIO5) and window (GPIO6)

You define the output GPIO12 as a BinarySwitch:

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop
H4P_BinarySwitch h4onof(12,ACTIVE_HIGH,OFF);
```

Even before adding anything else you can now switch on the light  when any source ( [Serial](h4cmd.md), [MQTT](h4mqtt.md), [HTTP REST](h4asws.md) or [linked GPIO input connector](h5gpio.md) ) sends an `on`,`off`,`toggle` or `switch` command.

Next, add the sensors and tie them all to the `BinarySwitch` `h4onof`

```cpp
H4P_GPIOManager h4gm;

void h4setup(){
    h4gm.LatchingSource(0,INPUT,ACTIVE_LOW,15); // 15ms debounce timeout
    h4gm.RetriggeringSource(4,INPUT,ACTIVE_HIGH,10000); // 10sec motion timeout
    h4gm.DebouncedSource(5,INPUT,ACTIVE_HIGH,15); // door alarm
    h4gm.DebouncedSource(6,INPUT,ACTIVE_HIGH,15); // window alarm
}
```

Now the light go on when any of the following happen:

* You press the button
* The door opens
* The window opens
* Motion is detected
* ON command recived from any source, e.g. MQTT etc

The light will go off when:

* You press the button again when its ON for any reason
* The door closes
* The window closes
* The PIR "times out" after 10 seconds.
* OFF command recived from any source, e.g. MQTT etc

## More advanced example

Now assume that instead of the light going ON you want your device to send you an SMS

1. Write the SMS function to send ON / OFF notifications.
2. Change the **H4P_BinarySwitch** to a **H4P_BinaryThing** and give it the name of your SMS function

That's it - the rest is the same.

## BinarySource vs BinarySwitch Summary

* BinarySource calls a user-defined function with `bool` value when `on`,`off`,`toggle` or `switch` command is received
* BinarySwitch drives a GPIO HIGH or LOW when  `on`,`off`,`toggle` or `switch` command is received

---

![H4P Flyer](/assets/switchthing.jpg) 
---
# Usage

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop
H4P_GPIOManager h4gm;
// H4P_AsyncWebServer h4asws(... if using H4P_UPNPServer
H4P_BinarySwitch h4onof;
// OR H4P_BinaryThing h4onof;
// Optionally:
// H4P_UPNPServer("Lounge Floor Lamp");
```

## Dependencies

* [H4P_GPIOManager](h4gm.md) Plugin

## Commands Added

* h4/off
* h4/on
* h4/switch/n (payload = 1 or 0)
* h4/toggle (invert current state)
* h4/state // report state

// H4P_UPNPServer only
* h4/upnp/name/N (payload N= new UPNP "friendly name")
  
## Topics automatically published

If [H4P_AsyncMQTT](h4mqtt.md) is also used, this plugin publishes `h4/< your device name >/state` with a payload set to the current state whenever the state changes

---

# API

```cpp
/* Constructors
pin is the GPIO output which gets "switched" when the state changes
sense is ACTIVE_HIGH or ACTIVE_LOW depending on the device
inital is the starting state ON or OFF
onChange is the name of  a user function that gets called after the state change with b set to the current state see GPIOManager plugins for more details
onConnect is called when UPNP_Server is fully "up"
onDisconnect is called when UPNP_Server goes "down"
timer ("AutoOFF") causes the device to turn off automatically after n milliseconds. When 0, stays ON till commanded OFF
*/
H4P_BinarySwitch(uint8_t pin,H4GM_SENSE sense, uint32_t initial,H4BS_FN_SWITCH f=nullptr,uint32_t timer=0)
H4P_BinaryThing(H4BS_FN_SWITCH f=nullptr,bool initial=OFF,uint32_t timer=0);

void turnOff();
void turnOn();
void toggle(); // invert state
void turn(bool b); // b = new state: 1 or 0; true/false; ON/OFF

// H4P_UPNPServer
H4P_UPNPServer(const string& name="",H4_FN_VOID onConnect=nullptr,H4_FN_VOID onDisconnect=nullptr);
void friendlyName(const string& name); // sets UPNP friendly name. Causes a reboot

```

# Output Examples

[Example Sketch - BinarySource](../examples/H4P_BinaryThing/H4P_BinaryThing.ino)
[Example Sketch - BinarySwitch](../examples/H4P_BinarySwitch/H4P_BinarySwitch.ino)
[Example Sketch - BinarySwitch with 3-function button](../examples/H4P_BinarySwitchmfnb/H4P_BinarySwitchmfnb.ino)
[Example Sketch - BinarySwitch with MQTT](../examples/H4P_BinarySwitchMQTT/H4P_BinarySwitchMQTT.ino)
[Example Sketch - UPNPServer with MQTT](../examples/H4P_SONOFF_Basic/H4P_SONOFF_Basic.ino)

---
# Input Examples

You need to read the [H4P_GPIOManager](h4gm.md) documentation before using these

[Example Sketch - AnalogThresholdSource](../examples/H4GM_DebouncedSource/H4GM_DebouncedSource.ino)
[Example Sketch - DebouncedSource](../examples/H4GM_DebouncedSource/H4GM_DebouncedSource.ino)
[Example Sketch - EncoderSource](../examples/H4GM_EncoderSource/H4GM_EncoderSource.ino)
[Example Sketch - LatchingSource](../examples/H4GM_LatchingSource/H4GM_LatchingSource.ino)
[Example Sketch - PolledSource](../examples/H4GM_PolledSource/H4GM_PolledSource.ino)
[Example Sketch - RawSource](../examples/H4GM_RawSource/H4GM_RawSource.ino)
[Example Sketch - RetriggeringSource](../examples/H4GM_RetriggeringSource/H4GM_RetriggeringSource.ino)

---

# Advanced Topics

## Installation of Windows components for UPNP variants

***N.B.** It seems Windows has a bug(!) in handling Wemo UPNP devices. Until this is fixed or a workaraound is found, the only way to activate your H4 device using UPNP is indirectly via MQTT*

Until such time as an installation script is written (soon, I promise :) ) getting the windows functionality is a bit of an ordeal, I'm afraid.

The first thing you need to do is to locate you Arduino library installation folder for H4Plugins, it will be something like `C:\Users\phil\Documents\Arduino\libraries\H4Plugins`

The files you will need are in the `src` subfolder

1. Install Powershell if you dont already have it and set it up so that it can run code without needing admin rights More information [here](https://superuser.com/questions/106360/how-to-enable-execution-of-powershell-scripts) 

2. Install [m2mqtt](https://github.com/eclipse/paho.mqtt.m2mqtt) You may find it easier to first install [nuget.exe](https://www.nuget.org/downloads) and run `nuget.exe install M2Mqtt -o c:\lib`

3. Edit h4p.reg and change the location in the final to match your username  Then right-click on that file and select "Merge" to add it to the registry

---

## Device naming of UPNPServer

If no name is given in the constructor, it defaults to "upnp XXXXXX" where XXXXXX is the unique chip ID of the device (usually the last 6 characters of the MAC address).

This is useful to enable a single generic sketch to be uploaded to numerous devices without change. Each device should then be sent a `h4/upnp/name` command to give it a "sensible" name. This can be done by any MQTT client using stored messages (or e.g. NODE-RED), so that each device gets its own new name every time it reboots. See [Advanced Topics](advanced.md)

---

## "Tweakables"

The following values are defined in `config.h` . They are chosen initally to set a good balance between stability, performance and memory / stack usage. *It is not advisable to change them unless you know exactly what you are doing and why*. 

**N.B.** **Support will not be provided if any of these values are changed.**

* H4P_UDP_JITTER

The number of milliseconds entropy between successive UPNP bradcasts, to minimise queue growth and UDP "flooding".

* H4P_UDP_REFRESH

The number of milliseconds "lifetime" between UPNP "keepalive" broadcasts

* H4P_UDP_REPEAT

The integer number of repeat UDP messages sent on each occasion to prevent packet loss

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
