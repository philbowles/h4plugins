![H4P Logo](../assets/DiagLogo.jpg)

# H4P_EventListener, H4P_SerialLogger

## Shortname ears< N >

Registers a handler function with a selected set of events. 

For information on how to use this plugin, see :gem:[Everything is an event: Listeners, Emitters and Handlers](events.md)

(`H4P_SerialLogger` is simply an `H4P_EventListener` with a predefined handler funstion that timply logs the event details to the Serial monitor)

---

## Contents

* [Usage](#usage)
* [Dependencies](#dependencies)
* [Commands Added](#commands-added)
* [Service Commands](#service-commands)
* [API](#api)

---

# Usage

H4P_EventListener is a multiple-use plugin: there can be many in one app. 
They will appears as <base shortname>1, <base shortname>2. <base shortname>3 etc. 
They may be instantiated as any name.

```cpp
H4P_EventListener myEL1(...             // internal ref: ears1
H4P_EventListener myEL2(...             // internal ref: ears2
H4P_EventListener anyNameYouChoose(...  // internal ref: ears3
...
H4P_SerialLogger slog(... // defaults to Serial Monitoe
```

# Dependencies

N/a

# Commands Added

N/A

# Service Commands

`stop` / `start` have no effect

## Callback functions

```cpp
/*
`svc` is a unique identifier of the origin that sent or "emitted" this event see the docs linked above for the valid values)
`type` is the type of event.
`msg` is optional information associated with that event.

typename H4P_FN_EVENTHANDLER
*/
void evt_handler(const string& svc,H4PE_TYPE type,const string& msg);
```

---

# API

## Constructor
/*
filter is Name of the event (or combination of events) to be fowarded to the event handler function
handleEvent is the callback function
*/

```cpp
H4P_EventListener(uint32_t filter,H4P_FN_EVENTHANDLER handleEvent);
...
H4P_SerialLogger(uint32_t filter); // default handler does Seril.printf( ... etc)
```

Numerous examples sketches include the `H4P_EventListener`, e.g.

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,H4_Q_CAPACITY,false) 

H4P_EventListener gpio(H4PE_GPIO,[](const string& pin,H4PE_TYPE t,const string& msg){
    int p=atoi(pin.c_str());
    int v=atoi(msg.c_str());
    switch(p){
      case D1:
        Serial.printf("P%d V = %d\n",p,v);
        break;
    }
});

h4pEncoder rotary(D1,D2);
```

[Example using more than 1 at a time](../examples/00_START_HERE/04_YourOwnEventHandling/04_YourOwnEventHandling.ino)


---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/es/esparto)