![H4P Logo](/assets/DiagLogo.jpg)

# H4P_VerboseMessages

## Shortname "vm"

Translates common command / control error codes, event names and other numeric stmbols into human-readable text. This generally happens automatically simply by including the plugin at the top of the sketch, but it may also prove useful for the user who wishes to provide his/her own diagnostics.

## Contents
* [Usage](#usage)
* [Dependencies](#dependencies)
* [Commands Added](#commands-added)
* [Events Emitted](#events-emitted)
* [Events Listened for](#events-listened-for)
* [Tasks Run](#tasks-run)
* [Service Commands](#service-commands)
* [API](#api)
    * [Global non-member functions](#global-non-member-functions)
    * [Callback functions](#callback-functions)
    * [Member Functions](#constructor)

---

# Usage

```cpp
H4P_VerboseCommands h4vm;
```

---

# Dependencies

None, but should precede all other plugins

---

# Commands Added

N/A

---

# Events Emitted

N/A

More information on [Event Emitters and Listeners](docs/../h4logs.md)

---

# Events Listened for

N/A

More information on [Event Emitters and Listeners](docs/../h4logs.md)

---

# Tasks Run

| Symbolic Name | Short Name | Type | Singleton | Purpose |
| :----------   | :--------- | :--- | :-------: | :---    |
|N/A| | | | |


---
# Service Commands

`stop` / `start` have no effect

---

# API

## Global Non-member functions

N/A

---

## Callback functions

```cpp
const char* giveTaskName(uint32_t id);
```

If the user wishes to provide names for his own tasks (to enable easy identification whe using `h4/show/q`) then he must provide this function which should perform a table lookup for the user-chosen taskID to its matching 4-character shortname.

:pushpin: User-chosen taksIDs should be below 50 so as not to clash with those already in use by H4Plugins.

Example:

```cpp
const char* giveTaskName(uint32_t n){
  static H4_INT_MAP mydata={
    {1,"Tick"},
    {4,"Rude"},
    {13,"Link"}, // don't have to be in order (but why not?)
    {6,"1Tim"}, // don't have to be used (but why not?)
    {49,"10GB"} // Keep them below 50 - IDs 50-99 are used by the system
    };
  return mydata.count(n) ? mydata[n].c_str():"ANON";
}
```

[Example Sketch](../examples/BASICS/H4_TaskNames/H4_TaskNames.ino)

---

## Constructor
```cpp
H4P_VerboseMessages();
```

string getEventName(uint32_t e){
    return eventTypes.count(e) ? eventTypes[e]:string("No such type (")+stringFromInt(e)+")";
}

string getTaskType(uint32_t e){
    return taskTypes.count(e) ? taskTypes[e]:stringFromInt(e,"?%02d?");
}

string getTaskName(uint32_t e){
    return uppercase(taskNames.count(e) ? taskNames[e]:giveTaskName(e));
}


---

## getErrorMessage

```cpp
string getErrorMessage(uint32_t e);
```

Returns text corresponding to error code `e` or "No such error ..." if `e is out of range`

---

## getEventName

```cpp
string getEventName(uint32_t e)
```

Returns text name of event `e` or "No such error ..." if `e is out of range`

---

## getErrorMessage

```cpp
string getErrorMessage(uint32_t e);
```

Returns text corresponding to error code `e` or "No such error ..." if `e is out of range`

---

## getErrorMessage

```cpp
string getErrorMessage(uint32_t e);
```

### Returns
text corresponding to error code `e` or "No such error ..." if `e is out of range`

---


(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Blog](https://8266iot.blogspot.com)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook IOT with ESP8266 (moderator)}](https://www.facebook.com/groups/1591467384241011/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)