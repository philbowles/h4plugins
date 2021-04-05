![H4P Logo](/assets/DiagLogo.jpg)

# Global functions and callbacks

* [Global Callbacks](#global-callbacks)
* Global Functions
  * [Global "lifecycle" functions](#global-lifecycle-functions)
  * [h4p Utilities](#h4p-utilities)
  * [h4p Internal (not recommended)](#h4p-internal-not-recommended)
* [Logging Macros](#logging-macros)
  
---

# Global Callbacks

These may be provided by the user if required

| Function | Source | Purpose |
| :--- | :--- | :--- |
|`const char* giveTaskName(uint32_t n);`|[h4vm](vm.md)| Translate user's task ID to meaningful name see  [Example Sketch](../examples/BASICS/H4_TaskNames/H4_TaskNames.ino)|
|`void h4UserLoop(void);`|[H4](https://github.com/philbowles/H4)| 1x per loop - use with caution, requires H4 option setting. see  [Example Sketch](../examples/BASICS/H4_TaskNames/H4_TaskNames.ino)|
|h4pGlobalEventHandler(const string& svc,H4PE_TYPE t,const string& msg);`|various| Catch-all event handler. See [Event Listeners, Event Emitters and logging](events.md)|
|`void onFactoryReset(void);`|various| Final tidying / freeing of resources immediatley before the event|
|`void onMQTTError(e,info);`|[MQTT](h4mqtt.md)| General MQTT error handler|
|`void onReboot(void);`|various| Final tidying / freeing of resources immediatley before the event|
|`void onUiChange(const string& name,const string& value);`|[WiFi](h4wifi.md)|If no specific field change function given|

---

# Global "lifecycle" functions

## h4FactoryReset

```cpp
h4FactoryReset();
```

Emits `H4PE_FACTORY` and once all listeners have completed, erases any stored configuration data (N.B. **NOT** the FS webserver files) then reboots the device

**WARNING** *this will delete all saved configuration data before doing a reboot - use with caution!*

### Equivalent command: h4/factory

---

## h4reboot

```cpp
h4reboot();
```

"*Does exactly what it says on the tin.*" Emits `H4PE_REBOOT` and once all listeners have completed, reboots the device


### Equivalent command: h4/reboot

---


# Global utility / internal functions

Functions starting `h4p` fall into two classes:

* Recommended for calling by user in the appropriate circumstances
* Used mostly internally and *not recommended* for general use

The latter category are included only for the sake of completeness and should never be required by the user. They will be required by advanced users wishing to write their own plugins.

## `h4p` Utilities

### Dealing with global variables

H4Plugins keeps a table of named variables. It uses these internally, but the user can also add his/her own. This is particularly useful when adding user items to the web user interface (webUI) since any named field that the user adds will have its value filled in at run-time from the global variable matching the field name. See the [webUI example](../examples/WEBUI) sketches.

***N.B.*** These variables exist only for the lifetime of the sketch, they are lost on reboot. If you need them to persist across reboots, see [H4P_PersistentStorage](h4stor.md)


```cpp
// create / update
void h4pSetConfig(const std::string& c,const std::string& v); // create / update global std::string variablr
void h4pSetConfig(const std::string& c,const int v); // create / update global int variable
// retrieve
std::string h4pGetConfig(const std::string& c); // return named global variable as a std::string
int h4pGetConfigInt(const std::string& c); // return named global variable as an int
// use as "template" replacements
/*
Imagine you have a string myHTML = "<html>%message%,%user%</html>"
and you have also called h4pSetConfig("message","HELLO"); and h4pSetConfig("user","Phil")
*/
std::string h4preplaceparams(const std::string& s);
/*
Then calling:
std::string readytosend=h4preplaceparams(myHTML);
Will return:
"<html>HELLO,Phil</html>"

NB the variable name must start AND end with % - be very careful, this is a simple function, 
if you do not have matched % signs, it WILL crash!!
*/
```

### Translating cryptic error codes / system IDs

To save space and improve performance, H4P identifies all internal entities with numeric codes. Normally the user will never need to come across these, but if they do, they will mean very little. When debugging and testing it is useful to see the human-readable equivalents - which *may* mean a little more.

If [H4P_VerboseMessages](vm.md) is in use, then the following functions will return human-readable values

```cpp
std::string h4pgetErrorMessage(uint32_t e); // command errors
std::string h4pgetEventName   (H4PE_TYPE e);
std::string h4pgetTaskType    (uint32_t e); // H4 Timer Task Type e.g. every, once, nTimes etc
std::string h4pgetTaskName    (uint32_t e); // H4 Timer Task name
```

Users can name their own tasks: see `giveTaskName` callback [above](#global-callbacks) and [Example Sketch](../examples/BASICS/H4_TaskNames/H4_TaskNames.ino) showing it in use

### Event Handling

```cpp
void h4pOnEvent(H4PE_TYPE t,H4P_FN_USEREVENT f);
```

Names a function `f` (or defines a lambda) to handle the specific event type `t`. If more than one event type needs to be monitored, use [H4P_EventListener](docs/events.md)

```cpp
void h4pUserEvent(const char* format,...);
```

Emits an event of type `H4PE_USER` with a message made up from `printf`-style format string and optional list of parameters, e.g. `h4pUserEvent("Something %s interesting happened","mildly");`

---

## `h4p` Internal (not recommended)

```cpp

template<typename T,typename... Args> T* h4pdepend(H4Service* me,H4PID p,Args... args); // dynamically load plugin and hook start/stop dependencies
void h4pemit(const string& svc,H4PE_TYPE t,const char* msg); // worker function of h4psysevent to avoid template code bloat
template<typename T> T* h4pisloaded(H4PID p); // returns nullptr if Plugi not in use
void h4pregisterhandler(const string& svc,uint32_t t,H4P_FN_EVENTHANDLER f); // adds an event handler function to the correct event chain
template<typename T,typename... Args> T* h4prequire(H4Service* me,H4PID p,Args... args); // // dynamically load plugin, no hook-in
template<typename... Args> void h4psysevent(const string& svc,H4PE_TYPE t,const std::string& fmt, Args... args);
void h4pupcall(H4Service* me,H4Service* ptr); // worker function of h4pdepend to avoid template code bloat

```

---

# Logging Macros

These are handy shortforms of `h4psysevent` calls with predefined values. All have `f` as a `printf`-style format string followed by optional parameters

```cpp
SEVENT(t,f,...); // Send event of type t from global context, i.e. NOT withing code of H4Service
PEVENT(t,f,...); // Send event of type t from within code of H4Service
```

The following `H4PE_MSG` emitters will be compiled out unless `H4P_LOG_EVENTS` is set to 1 in [`config.h`](../src/config.h)

```cpp
SLOG(f,...); // Emit message event from global context, i.e. NOT within code of H4Service
XLOG(f,...); // Emit message event from within code of H4Service
```

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)