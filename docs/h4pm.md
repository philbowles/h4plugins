![H4P Flyer](../assets/GPIOLogo.jpg)

# H4P_PinMachine

## Service shortname gpio

Provides GPIO management and centralises control of any defined NODE-PINK functions.

For further information on how to use this plugin see 
* [GPIO Handling and "NODE-PINK" Introduction](nodepink.md) :heavy_check_mark:
  * [GPIO/NODE-PINK: a logical approach](logphys.md) :heavy_check_mark:
  * [GPIO/NODE-PINK: basic flows](basic.md) :heavy_check_mark:
  * [GPIO/NODE-PINK: rotary encoder flows](encoders.md) :heavy_check_mark:
  * :building_construction: [GPIO/NODE-PINK: analog flows](analog.md)
  * :building_construction: [GPIO/NODE-PINK: advanced techniques](nodepinkadv.md)

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
H4P_PinMachine bally(...
```

This plugin is a "singleton" - there may be only one single instance of it in the app. 
It may be instantiated as any name the user chooses, prefix all API calls below with that name.

# Dependencies

N/A

# Service Commands

`stop` will disconnect from PinMachine and initiate closedown of all Plugins the depend on PinMachine
`start` will connect to PinMachine and start webserver and all dependent services

---

## API

For guidance on the use of the `ui...` functions, see [Customising the web UI](webui.md)

```cpp
/*
Constructor
*/
H4P_PinMachine(): 
/*
Common parameters:
p = HPIO pin number
b = a boll ON or OFF
*/
void all(bool b); // logicalWrite(b) to ALL managed GPIO pins
uint8_t logicalRead(uint8_t p); // return LOGICAL value of pin (normalised for sense ACTIVE_LOW / _HIGH)
void logicalWrite(uint8_t p,bool b); // return LOGICAL value of pin (normalised for sense ACTIVE_LOW / _HIGH) 
int getValue(uint8_t p); // return internal value conceptual pin can be -ve, ny number up to 4GB depending on pin type
msg inject(uint8_t p,uint32_t metal,bool timer=false); // used internally, user should never need to call this
bool isAnalog(uint8_t p); // true if pin is configured as an Analog pin
h4pGPIO* isManaged(uint8_t p){ return h4pPinMap.count(p) ? h4pPinMap[p]:nullptr; } // true if pin has h4pGPIO object assigned to it
bool isOutput(uint8_t p); // true is assigned h4pGPIO is in OUTPUT mode
H4_TIMER periodicRead(uint8_t p,uint32_t f); // used internally, user should never need to call this
msg runFlow(uint8_t p,msg); // used internally, user should never need to call this

```

# Example sketches

Extensive examples can be found in the [examples/01_GPIO_PIN_MACHINE](../examples/01_GPIO_PIN_MACHINE) folder
  
---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)