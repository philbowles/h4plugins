![H4P Flyer](/assets/GPIOLogo.jpg) 

# "Things", "Switches", "Sources" and "Slaves"

Essential background information before using any of:

* [H4P_BinaryThing](swings.md)
* [H4P_ConditionalThing](swings.md)
* [H4P_BinarySwitch](swings.md)
* [H4P_ConditionalSwitch](swings.md)

---

# What is a "Thing"?

Let's not forget that "IOT" is the Internet of *Things*. So what is a "Thing"? The answer is: *anything you want it to be*. In H4 Plugins terms, it is an object that holds an internal `state` of `ON` or `OFF` and reacts to commands `on`,`off`,`toggle` , `switch` and `auto`. It provides a fat-finger-friendly graphcial on/off button (The "big red switch") in the webUI if you use [H4P_WiFi](h4wifi.md) and if you are using the  [H4P_AsyncMQTT plugin](h4mqtt.md) it automatically publishes its internal `state` whenever it changes.

If your app only needs to switch a single GPIO like a relay or a lamp, then a "Switch" is what you need. A Switch is a just a Thing whose only action is to feed the on/off commands to a single selceted GPIO pin, whereas "Things" can do whatever they want when switched on: play a tune, send a message to a remote server, grab a snapshot from a security camera etc...

"Things" drive user-defined functions, "Switches" drive single GPIOs - in every other respects they behave exactly the same way. You can turn them on or off, that's pretty much it. They also have an "auto off" feature (which is great for energy-saving) which will turn them off after a user-defined amount of time, no matter how the got switched on. If you leave it at zero the Thing or Switch will stay in the ON state until commanded OFF in some other way.

# Linking your Thing/Switch to input events: "Sources"

There can only be one *output* Thing (or Switch) in a sketch/app and it must be named as `h4onof`. It is the default handler for any `on` or `off` command. On the *input* side of things, you can have as many ways as you like to trigger it.

You should already be familiar with [Serial Commands](h4p.md), [MQTT](h4mqtt.md), [HTTP REST](h4wifi.md), [UPNP device](upnp.md) (e.g. Amazon Alexa voice command) as ways to send `on`,`off`,`toggle` or `switch` commands to you device, now there are many more, called "Sources".

A Source is simply a plugin that automatically sets the default output `h4onof` plugin to same as the Source's own state. It is a [linked GPIO input connector](h5gpio.md).

You can have many as you like all linked to the single *output* `h4onof`. Each of these behaves exactly like its non-Source counterpart except that instead of calling back your code when its state changes, it automatically sends its ON or OFF state to the `h4onof` handler, either a xxxSwitch or a xxxThing.

* AnalogThresholdSource [Example Code](../examples/GPIO/H4GM_AnalogThresholdSource/H4GM_AnalogThresholdSource.ino)
* DebouncedSource [Example Code](../examples/GPIO/H4GM_DebouncedSource/H4GM_DebouncedSource.ino)
* EncoderSource [Example Code](../examples/GPIO/H4GM_EncoderSource/H4GM_EncoderSource.ino)
* LatchingSource [Example Code](../examples/GPIO/H4GM_LatchingSource/H4GM_LatchingSource.ino)
* PolledSource [Example Code](../examples/GPIO/H4GM_PolledSource/H4GM_PolledSource.ino)
* RawSource [Example Code](../examples/GPIO/H4GM_RawSource/H4GM_RawSource.ino)
* RetriggeringSource [Example Code](../examples/GPIO/H4GM_RetriggeringSource/H4GM_RetriggeringSource.ino)

In addition, you may also have:

* [h4pMultifunctionButton](h4mfnb.md) [Example Code](../examples/XTRAS/H4P_SONOFF_Basic/H4P_SONOFF_Basic.ino)
* [UPNPServer](upnp.md)

The [h4pMultifunctionButton](h4mfnb.md) provides an easy way to switch your Thing/Switch with a physical button. The "Multi-function" aspect of it also allows you to reboot the device or factory reset it, depending on how long you hold the button down. A simple short "stab" though serves as your devices on/off button.

The [UPNPServer](upnp.md) provides (among other things) the abiity to detect Amazon "Alexa" voice commands an pass them to the Thing/Switch

## Simple Example with a ...Switch

Assume you have an output GPIO on pin 12 wired to a relay that controls a security light

Now add some inputs:

* Pushbutton on GPIO0
* PIR sensor on GPIO4
* Magnetic reed switches on front door (GPIO5) and window (GPIO6)

You define the output GPIO12 as a BinarySwitch:

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop
H4P_BinarySwitch h4onof(12,ACTIVE_HIGH);
```

Even before adding anything else you can now switch on the light  when any source ( [Serial](h4p.md), [MQTT](h4mqtt.md), [HTTP REST](h4wifi.md) or [UPNP device](h4upnp) (e.g. Amazon Alexa voice command) sends an `on`,`off`,`toggle` or `switch` command.

Next, add the sensors as [linked GPIO input connector](h5gpio.md)s or "Sources"

```cpp
H4P_PinMachine h4gm;

void h4setup(){
    h4gm.LatchingSource(0,INPUT,ACTIVE_LOW,15); // 15ms debounce timeout
    h4gm.RetriggeringSource(4,INPUT,ACTIVE_HIGH,10000); // 10sec motion timeout
    h4gm.DebouncedSource(5,INPUT,ACTIVE_HIGH,15); // door alarm
    h4gm.DebouncedSource(6,INPUT,ACTIVE_HIGH,15); // window alarm
}
```

Now when any of the sources changes to ON, it will set the Thing/Switch to ON. Of course the same is true when any of them turns OFF, so be careful to choose sources that won't "fight" with each other in real life and cause you device to "hammer" on and off rapidly!

In our example, the light will go on when any of the following happen:

* You press the button (LatchingSource on GPIO0)
* The door opens (DebouncedSource on GPIO5)
* The window opens (DebouncedSource on GPIO6)
* Motion is detected (DebouncedSource on GPIO4)
* ON command recived from any source, e.g. MQTT etc

The light will go off when:

* Auto-off timer expires (change the value with the auto command: 0=never)
* You press the button again
* The door closes
* The window closes
* The PIR "times out" after 10 seconds.
* OFF command received from any source, e.g. MQTT etc

## "...Thing" example

Now assume that instead of the light going ON you want your device to send you an SMS

1. On an SMS-equipped device, write the SMS function to send ON / OFF notifications.
2. Change the **H4P_BinarySwitch** to a **H4P_BinaryThing** and give it the name of your SMS function

That's it - the rest is the same - you'll get an SMS message when any of the ON or OFF events occurs, instead of the secuirty light turning on / off.

## Thing vs Switch Summary

* BinaryThing calls a user-defined function with `bool` parameter when `on`,`off`,`toggle` or `switch` command is received
* BinarySwitch drives a GPIO HIGH or LOW when  `on`,`off`,`toggle` or `switch` command is received
* ConditionalThing is a BinaryThing that only operates when user-defined function returns `true`
* ConditionalSwitch is a BinarySwitch that only operates when user-defined function returns `true`
* Only **one** Thing or Switch per app
* It must be called `h4onof`

This picture is slightly out-of-date but should give you an idea of how these all "hang together"

![H4P Flyer](/assets/switchthing.jpg)

---

# Slaves

Things and Switches can be linked to each other so that when one turns on, it automatically triggers another to turn on too. You can link groups of similar devices together in this way. The controlling device is the "masster" and the automatically linked device is the "slave".

This - of course - also works for "off". If the "master" has an auto-off time set, then all of it slaves will behave as if they did too, even though they may not behave that way when switched directly. That is unless each slave has it own auto-off setting...

The important thing to realise is that the master does not control / override the slaves directly, it simply "forwards" its own state to them as a command. So if a master with no auto-off timer switches on a slave and that slave has its own specific auto-off time of 5 seconds, then it will go off after five seconds, even though the master stays on "forever".

By the same token if the master has a *shorter* auto-off time than the slave - say 3 seconds in the above example - then all the slaves will go off after 3 seconds whether they have their own 5 second auto-off timers or not!

When you use any of these in your app, they automatically add `h4/slave` to the list of available commands. See [Things/Switches API](swings.md) for details on how to use it.

---

(c) 2020 Phil Bowles h4plugins@gmail.com

* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)