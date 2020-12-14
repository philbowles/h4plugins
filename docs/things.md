![H4P Flyer](/assets/GPIOLogo.jpg) 

# BinarySwitch, BinaryThing, ConditionalSwitch, ConditionalThing (shortname="onof")

## Link auto/on/off/toggle/switch commands to a GPIO or functional object

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---

# What do they do?

Let's not forget that "IOT" is the Internet of *Things*. So what is a "Thing"? The answer is: *anything you want it to be*. In H4 Plugins terms, it is an object that holds an interal `state` of `ON` or `OFF` and reacts to commands `on`,`off`,`toggle` and `switch`. If you are using the  [H4P_AsyncMQTT plugin](h4mqtt.md) it publishes the `state` topic whenever the state changes. It also provides a fat-finger-friendly graphcial on/off button.

A Switch is a just a Thing with a predefined output function linked to a single GPIO.

There can only be one *output* Thing (or Switch) in a sketch/app and it must be named as `h4onof`. It is effectively the default handler for the above commands. It is what your device does when any source ( [Serial](h4cmd.md), [MQTT](h4mqtt.md), [HTTP REST](h4asws.md), Amazon Alexa voice command or [linked GPIO input connector](h5gpio.md) ) sends an `on`,`off`,`toggle` or `switch` command.

You can have many *input* [linked GPIO input connector](h5gpio.md)s all linked to the single *output* `h4onof`. Each of these behaves exactly like its non-Source counterpart except that instead of calling back your code when its state changes, it automatically sends its ON or OFF state to the `h4onof` handler, either a xxxSwitch or a xxxThing.

* AnalogThresholdSource [Example Code](../examples/GPIO/H4GM_AnalogThresholdSource/H4GM_AnalogThresholdSource.ino)
* DebouncedSource [Example Code](../examples/GPIO/H4GM_DebouncedSource/H4GM_DebouncedSource.ino)
* EncoderSource [Example Code](../examples/GPIO/H4GM_EncoderSource/H4GM_EncoderSource.ino)
* LatchingSource [Example Code](../examples/GPIO/H4GM_LatchingSource/H4GM_LatchingSource.ino)
* PolledSource [Example Code](../examples/GPIO/H4GM_PolledSource/H4GM_PolledSource.ino)
* RawSource [Example Code](../examples/GPIO/H4GM_RawSource/H4GM_RawSource.ino)
* RetriggeringSource [Example Code](../examples/GPIO/H4GM_RetriggeringSource/H4GM_RetriggeringSource.ino)

In addition, you may also have:
* H4P_MultiFunctionButton [Example Code](../examples/XTRAS/H4P_SONOFF_Basic/H4P_SONOFF_Basic.ino)
* H4P_UPNPServer [Example Code](../examples/XTRAS/H4P_SONOFF_Basic/H4P_SONOFF_Basic.ino)

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

## xxxThing vs xxxSwitch Summary

* BinaryThing calls a user-defined function with `bool` value when `on`,`off`,`toggle` or `switch` command is received
* BinarySwitch drives a GPIO HIGH or LOW when  `on`,`off`,`toggle` or `switch` command is received
* ConditionalThing is a BinaryThing that only operates when user-defined function returns `true`
* ConditionalSwitch is a BinarySwitch that only operates when user-defined function returns `true`

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
// OR H4P_ConditionalThing h4onof;
// OR H4P_ConditionalSwitch h4onof;
// Optionally:
// H4P_UPNPServer("Lounge Floor Lamp");
```

## Dependencies

* [H4P_GPIOManager](h4gm.md) Plugin

## Commands Added

* h4/auto/T (automatic off time T milliseconds)
* h4/off
* h4/on
* h4/switch/n (payload n = 1 or 0)
* h4/toggle (invert current state)
* h4/state // report state

H4P_UPNPServer only

* h4/upnp/N (payload N= new UPNP "friendly name")
  
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
predicate is the name of a bool returning function which takes a bool parameter of current state: if it returns false, switch/thing will ignore all commands
*/
H4P_BinarySwitch(uint8_t pin,H4GM_SENSE sense, uint32_t initial,H4BS_FN_SWITCH f=nullptr,uint32_t timer=0)
H4P_BinaryThing(H4BS_FN_SWITCH f=nullptr,bool initial=OFF,uint32_t timer=0);
H4P_ConditionalSwitch(uint8_t pin,H4GM_SENSE sense, uint32_t initial,H4_FN_CPRED predicate=nullptr,H4BS_FN_SWITCH f=nullptr,uint32_t timer=0):
H4P_ConditionalThing(H4_FN_CPRED predicate=nullptr,H4BS_FN_SWITCH f=nullptr,bool initial=OFF,uint32_t timer=0): 

void autoOff(uint32_t T); // change autoOff time to T milliseconds
void syncCondition(); // Conditional switch / thing only: keep webUI in-step with current value of condition
void turnOff();
void turnOn();
void toggle(); // invert state
void turn(bool b); // b = new state: 1 or 0; true/false; ON/OFF

// H4P_UPNPServer
H4P_UPNPServer(const string& name="",H4_FN_VOID onConnect=nullptr,H4_FN_VOID onDisconnect=nullptr);
void friendlyName(const string& name); // sets UPNP friendly name. Causes a reboot

```

For details on setting up Windows for the full UPNP experience, see [Advanced Topics](advanced.md)

# Output Examples

[Example Sketch - BinaryThing](../examples/THINGS/H4P_BinaryThing/H4P_BinaryThing.ino)
[Example Sketch - BinarySwitch](../examples/THINGS/H4P_BinarySwitch/H4P_BinarySwitch.ino)
[Example Sketch - BinarySwitch with 3-function button](../examples/THINGS/H4P_BinarySwitchmfnb/H4P_BinarySwitchmfnb.ino)
[Example Sketch - BinarySwitch with MQTT](../examples/THINGS/H4P_BinarySwitchMQTT/H4P_BinarySwitchMQTT.ino)
[Example Sketch - UPNPServer with MQTT](../examples/XTRAS/H4P_SONOFF_Basic/H4P_SONOFF_Basic.ino)
[Example Sketch - ConditionalThing](../examples/THINGS/H4P_ConditionalThing/H4P_ConditionalThing.ino)
[Example Sketch - ConditionalSwitch](../examples/THINGS/H4P_ConditionalSwitch/H4P_ConditionalSwitch.ino)

---
# Input Examples

You need to read the [H4P_GPIOManager](h4gm.md) documentation before using these

[Example Sketch - AnalogThresholdSource](../examples/GPIO/H4GM_DebouncedSource/H4GM_DebouncedSource.ino)
[Example Sketch - DebouncedSource](../examples/GPIO/H4GM_DebouncedSource/H4GM_DebouncedSource.ino)
[Example Sketch - EncoderSource](../examples/GPIO/H4GM_EncoderSource/H4GM_EncoderSource.ino)
[Example Sketch - LatchingSource](../examples/GPIO/H4GM_LatchingSource/H4GM_LatchingSource.ino)
[Example Sketch - PolledSource](../examples/GPIO/H4GM_PolledSource/H4GM_PolledSource.ino)
[Example Sketch - RawSource](../examples/GPIO/H4GM_RawSource/H4GM_RawSource.ino)
[Example Sketch - RetriggeringSource](../examples/GPIO/H4GM_RetriggeringSource/H4GM_RetriggeringSource.ino)

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
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook IOT with ESP8266 (moderator)}](https://www.facebook.com/groups/1591467384241011/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)
