![H4P Logo](../assets/DiagLogo.jpg)

# H4P_SerialCmd

## Shortname "cmd"

Provides common command / control functions to all other plugins and allows control of device from serial console.

---

## Contents

* [Usage](#usage)
* [Dependencies](#dependencies)
* [Commands Added](#commands-added)
* [Service Commands](#service-commands)
* [API](#api)

---
# Usage

None: Included automatically by the mandatory opening sequence.
```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,10,false)
```

This plugin is a "singleton" - there may be only one single instance of it in the app. It is called `h4p`, prefix all API calls below with `h4p.`

---

# Dependencies

N/A 

---

# Commands Added

See also [Common Command and Control: H4P_SerialCmd and Services](ccc.md)

* `h4/clear` (erase all global variables USE WITH CAUTION!!!) 
* `h4/config` (change value of on or more global variables* (see below)) 
* `h4/dump/x` (payload x = FS file name. Show contents of file.) 
* `h4/factory` ( "factory resets" the device)
* `h4/get/x` ( show the value of global variable x)
* `h4/reboot` ( reboots the device )
* `h4/show/all` (runs all the commands starting `h4/show/`)
* `h4/show/config` (internal configuration variables)
* `h4/show/fs`  (show all LittleFS filesystem files + sizes and used / free space)
* `h4/show/heap` ( result if _HAL_freeHeap()) 
* `h4/show/plugins` lists all the currrently loaded plugins
* `h4/show/q` (shows all tasks in the Q waiting to be scheduled)
* ... service control commands (see above)
* `help`

*config payload:

For a single variable the payload should be name=value. You may also update several variables at a time by separating those name=value pairs with commas, e.g.

```cpp
h4/config/myvar=42
h4/config/myvar=666,life=42,firmware=h4plugins
```

*** N.B. ***

*The correct operation of the system depends on many of the global variables. Several of those critical values will immediately cause side effects (possibly undwanted, e.g. reboot) if changed. Change only those you created yourself!*

---
# Service Commands

SerialCmd can be safely stopped - it will continue to provide command functionality to other plugins that depend upon it. e.g. [(http "REST")](h4wifi.md) and [**H4P_AsyncMQTT**](h4mqtt.md) but it will no longer accept serial commands on its own behalf. Thus it can only be stopped *once* from the serial console*. Unloading `cmd` will show a huge performance improvement (+50% ish), so it is worth considering once testing is complete, especially for devices that will be deployed remotely and will never be able to recive serial commands. (see autoStop parameter above)

(* It can be restarted remotely, e.g. via MQTT or http/REST)

---

# API

## Constructor

```cpp
/* Information only: this is instantiated automatically and should never be called by the user
autoStop = true will stop the service from listening to serial commands and improve performance by about +35%
*/
H4P_SerialCmd(bool autoStop=false);
```

---

## addCmd

```cpp
void addCmd(const string& name,uint32_t owner, uint32_t levID,H4_FN_MSG f=nullptr);
```

Adds a new command to the existing set: This is how plugins update the command table when they load. The user may also add his / her own commands, however this requires in-depth knowledge of the internal data structure and is described in detail in [Advance Topics](advanced.md)

---

## all

```cpp
void all();
```

Runs all commands from any loaded plugins containing "show", i.e. `h4/show/q` ... etc and also runs `h4/svc/state/...` on all loaded plugins

### Equivalent command: h4/show/all

---

```cpp
void clear();
```

Erases ***all*** global variables. USE AT YOUR PERIL! (if at all)

### Equivalent command: h4/clear

---

## gvXXXX functions

See also [Global Variables and persistent storage](globals.md)

```cpp
// The Global variables function set:
void gvDec(const string& name); // decrements variable expected to be numeric

void gvErase(const string& name); // remove a variable from the global store

void gvErase(initializer_list<const char*> nil); // remove several variables at once from the global store

bool gvExists(const string& name); // true if name is in the global store

int gvGetInt(const string& name); // return expected numeric value of name

string gvGetstring(const string& name); // return expected string value of name

void gvInc(const string& name); // increments variable expected to be numeric

void gvSave(const string& name); // changes temp / permanent flag to permanet and forces update flush

void gvSave(initializer_list<const char*> sav); // as above, more effifient for several at a time

void gvSetInt(const string& name,int value,bool save=false);// set value to int

void gvSetstring(const string& name,const string& value,bool save=false); // set vallue to string
```

---

## heap

```cpp
void  heap();
```

Shows the contents of H4's free heap

### Equivalent command: h4/show/heap

---

## help

```cpp
void help();
```

Shows the lst of available commands

### Equivalent command: help

---

## invokeCmd

```cpp
uint32_t invokeCmd(string command,string payload="",const char* src=userTag());
uint32_t invokeCmd(string,uint32_t payload,const char* src=userTag()); 
```

Runs the named `command` with a textual or integer `payload` (if required). `src` names / defines a "source" of the command. This is normally best left as the default "user" denoting that it was initiated from within user-code.

### Returns
an error code as described [above](#error-codes)

---

## plugins

```cpp
void plugins();
```

Lists all currently loaded plugins, and calls `show()` on each

### Equivalent command: h4/show/plugins

---

## read

```cpp
string read(const string& filename);
```

### Returns
The contents of the LittleFS file given by `filename`

---

## removeCmd

```cpp
void removeCmd(const string& name,uint32_t _subCmd=0); 
```

Removes a command from the existing set: This requires in-depth knowledge of the internal data structure and is described in detail in [Advance Topics](advanced.md)

---

## showFS

```cpp
void showFS();
```

Lists the contents of the LittleFS file system

### Equivalent command: h4/show/fs

---

## showQ

```cpp
void showQ();
```

Lists the contents of the H4 task queue

### Equivalent command: h4/show/q

---

## write

```cpp
uint32_t write(const string& filename,const string& data,const char* mode="w");
```

Writes the contents of `data` the LittleFS file given by `filename`. Modes are as described in the [LittleFS documentation](link?)

### Returns
A result as as described in the [LittleFS documentation](link?)

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)