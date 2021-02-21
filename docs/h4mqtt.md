![H4P Flyer](/assets/MQTTLogo.jpg) 

# H4P_AsyncMQTT

## Shortname mqtt

Connects to any MQTT server and manages automatic reconnection after failure. Provides utility functions to simplify message handling / parsing. User will find essential pre-reading [Handling MQTT topics / commands](mqcmds.md)

---

## Contents

* [Usage](#usage)
* [Dependencies](#dependencies)
* [Commands Added](#commands-added)
* [Events Emitted](#s-emitted)
* [Events Listened for](#s-listened-for)
* [Tasks Run](#tasks-run)
* [Service Commands](#service-commands)
* [API](#api)

---
# Usage

```cpp
H4P_WiFi h4wifi(...
H4P_AsyncMQTT cutie(...
```

This plugin is a "singleton" - there may be only one single instance of it in the app. 
It may be instantiated as any name the user chooses, prefix all API calls below with `myChosenName.`

## Dependencies

* [H4P_WiFi](h4wifi.md)

## Commands Added

* `h4/mqtt/change/w,x[,y,z]` (payload: w=newbroker, x=newport, y=newusername, z=newpassword)
* `h4/mqtt/report` // display basic details about the device

# Events Listened for

`H4P_EVENT_FACTORY`

# Events Emitted

`H4P_EVENT_MQTT_ERROR`

# Tasks Run

| Symbolic Name | Short Name | Type | Singleton | Purpose |
| :----------   | :--- | :--- | :-------: | :---    |
|H4P_TRID_MQMS|MQMS|QF|:heavy_check_mark:|MQTT topic message RX|
|H4P_TRID_MQRC|MQRC|every|:heavy_check_mark:|Reconnector|

# Service Commands

`stop` will disconnect from MQTT and initiate closedown of all Plugins the depend on MQTT
`start` will connect to MQTT and start all dependent services

# Callbacks

```cpp
void onConnect(void);
void onDisconnect(void);
uint32_t onMessage(vector<string>); // where "onMessage" is a user-defined topic handling function
void onMQTTError(uint8_t e,int info); // unreoverable PangolinMQTT error
```

# Topics automatically published

Publishes `h4/< your device name >/offline` when it loses the MQTT connection.

It publishes `h4/< your device name >/report` with a JSON payload of

```JSON
{
    "bin": "Generic_SONOFF.ino",
    "board": "D1_MINI",
    "h4P": "1.0.1",
    "h4UI": "1.0.1",
    "ip": "192.168.1.118",
    "pmv": "1.0.1"
}
```

Immediately after connection to the server.

These allows 3rd party software e.g. NODE-RED to maintain a list of all devices on the network and gain sufficient information from each to query and / or control their status. For example, this is a screenshot of the authors' NODE RED server see [Advanced Topics](advanced.md) for more details

![nodred](../assets/nodered.jpg)

---

# API

```cpp
/*
Constructor

optional Last Will and Testament structure:
*/
struct H4P_LWT {
    const char*     topic;
    const char*     payload;
    int             QOS;
    bool            retain;
};
//
// broker = name of server, either "somerserver.com" or "192.168.1.69" formats
// port = server port, usually 1883
// user / pass optional if server requires authentication
// onConnect = user callback when MQTT connects
// onDisconnect = user callback when MQTT disconnects
// 
// lwt = optional Last Will and Testament for custom LWT message
//
H4P_AsyncMQTT(string broker,uint16_t port, string user="",string pass="",H4_FN_VOID onC=nullptr,H4_FN_VOID onD=nullptr,H4P_LWT lwt={"","",0,false}):
//
void addReportingItem(const string& ri); // adds global config item ri to the list of values reported on `report`
void change(string broker,uint16_t port); // switch to new MQTT broker
void publishDevice(string topic,string payload=""); // publish <device>/topic with string payload
void publishDevice(string topic,uint32_t payload); // publish <device>/topic with numeric payload
void report(); // publishes golbal config item values as JSON
void subscribeDevice(string topic,H4_FN_MSG f); // call f when <device>/topic message received
void unsubscribeDevice(string topic);
```

## Example sketches

* [Subtopics](../examples/MQTT/H4P_MQTT_Subtopics/H4P_MQTT_Subtopics.ino)
* [Wildcards](../examples/MQTT/MQTT_Wildcards/MQTT_Wildcards.ino)
* [Custom Last Will and Testament](../examples/MQTT/H4P_MQTT_CustomLWT/H4P_MQTT_CustomLWT.ino)
  
---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)