![H4P Flyer](../assets/WiFiLogo.jpg) 

# H4P_LinkMaster

## Shortname link

Manages the Master / Slave linkage of two separate H4 devices. This allows one device to automatically control another (or indeed several), provided they all contain a Switch / Thing plugin (see ["Things" and "Switches"](things.md) ) and use MQTT (see [H4P_AsyncMQTT](h4mqtt.md))

In that case, the "Master" will need to include the `H4P_LinkMaster` plugin and simply forwards any change of state to all of its "slaves". No extra code is required by the slaves.

---

# Contents

* [Caveats](#caveats)
* [Dependencies](#dependencies)
* [Commands Added](#commands-added)
* [Service Commands](#service-commands)
* [API (for "Roamers")](#api-for-roamers)

---

# Caveats

Since `H4P_LinkMaster` simply forwards any change of state to the slaves, their own configuration will effect the exact behaviour of the system as a whole.

For example if a slave has an `autoOff` value of 5000 (=5 seconds) it will still turn OFF automatically after 5 secs even if "slaved" ON by a "Master".

Similarly if a slave has conditional Switch / Thing and the condition is currently `false` it will still refuse any ON/OFF commands, even though commanded by a "Master".

Finally be very careful to avoid cycles or loops.. eg. A slaves B which slaves C which then slaves A... as your house lights may end up looking like a christmas tree or blowing a circuit breaker...

---

# Usage

This plugin is a "singleton" - there may be only one single instance of it in the app. 
It may be instantiated as any name the user chooses.

```cpp
H4P_LinkMaster lynx(...);
```

# Dependencies

 [H4P_AsyncMQTT](h4mqtt.md)

# Commands Added

* `h4/slave/x,y` (payload x = H4 Device, b=0: Add slave, b=1: remove slave)
  
# Service Commands

`stop` / `start` have no effect

## Callback functions

N/A

---

# API

```cpp
/*
Constructor
slaves = unordered_set<string> containing names of H4 devices to be slaved, if these are known in advance
*/
H4P_LinkMaster(unordered_set<string> slaved={});
//
slave(const string& slave,bool inout=true); // slave = other H$ device name, inout=true adds slave, false removes it
```

[Example Sketch 1](../examples/05_THINGS_SWITCHES/H4P_ConditionalThing/H4P_ConditionalThing.ino)

[Example Sketch 2](../examples/XTRAS/Generic_SONOFF/Generic_SONOFF.ino)

[Example Sketch 3](../examples/XTRAS/H4P_Generic_SQUAWK/H4P_Generic_SQUAWK.ino)


---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/es/esparto)