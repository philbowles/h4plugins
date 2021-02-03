![H4P Logo](/assets/DiagLogo.jpg)

# Events, Emitters and Listeners

Essential background before using the events and/or logging systems

---

# Contents

* [Definitions](#defintions)
* [Anatomy of an Event Handler](#anatomy-of-an-event-handler)
* [Event Listeners](#event-listeners)
* [Event Emitters](#event-emitters)
* Formal API Specifications - Listeners
  * [H4P_EventListener](ears.md)
  * [H4P_SerialLogger](slog.md)
  * [H4P_LocalLogger](llog.md)
  * [H4P_MQTTLogger](mlog.md)
  * :building_construction: mysqllogger
* Formal API Specifications - Emitters
  * [H4P_EmitHeap](heap.md)
  * :building_construction: [H4P_EmitLoopCount](loops.md)
  * :building_construction: [H4P_EmitQ](eq.md)
  * :building_construction: [H4P_EmitTick](tick.md)

---

# Defintions

If you have used javascript, these concepts should already by familiar to you, but read on to ensure you understand H4Plugins use of the terminology. Similarly if you are familiar with MQTT, H4Plugins events are much like topics in the publish / subscribe model.

You should know already from reading the [H4 documentation](https://github.com/philbowles/H4) why we need "Event-driven programming", but what exactly *is* an event?

The answer may sound too vague, but an event is anything that happens in the real world - or your own code - that users or other plugins may be be interested in. The usual reason for being "interested" is that they may need to *do* something when that event occurs.

Let's take a simple example and assume that you want to reset your device to its "factory settings". There are several plugins that save configuration data. Each one of them needs to be told that a factory reset is in progress so they can clear up their own data. Because it is *you, the user* who chooses which plugins get loaded, H4Plugins needs a flexible system to call only those that are in use to tell them to clean up.

It does this by creating / sending or "emitting" an event - specifically `H4P_EVENT_FACTORY`. It just throws it into the air, like a red distress flare. It knows that anybody who sees the flare and recognises its significance will know there is suddenly work to do...

Every plugin, when it starts up, tells H4Plugins which events it is interested in (in our analogy, what colour flare(s) it will respond to). It has some code / process associated with the event known as an "event handler" and we call this declaration of interest "registering an event". The plugin is now an "Event Listener". Anything that can send an event is an "Event Emitter" - it is perfectly possible to be both at the same time.

---

# Anatomy of an Event Handler

Technically speaking, an Event Listener is *any* piece of code that registers a handler for a known event and provides a callback function of the form:

```cpp
void MyVeryOwnEventHandler(H4PID pid,H4P_EVENT_TYPE t,const string& msg){ ... do some interesting stuff }
```

All plugins do this automatically, so all plugins are - by definition - Event Listeners. Whether they actually choose to listen / react is another matter: many don't take advantage of the feature as they don't have any need / interest in any of the events.

* `pid` is the plugin ID of the plugin that sent or "emitted" this event. Most of the time it doesn't matter *who* caused the event, but it can be useful when trying to track down bugs.
* `t` is the type of event. See the table below

* `msg` is optional information about that event. It is different for every event type

## Events

| Event Name | Meaning | Msg contains | Note |
| :--- | :--- | :--- | :---: |
|H4P_EVENT_NOOP|Nothing||1|
|H4P_EVENT_MSG|Arbitrary message|user-defined|2|
|H4P_EVENT_SVC_UP|Plugin service up|shortname|3|
|H4P_EVENT_SVC_DOWN|Plugin service down|shortname|3|
|H4P_EVENT_CMD|A command was entered|The command|3|
|H4P_EVENT_USER|User event|user-defined||
|H4P_EVENT_HEAP|Heap has been measured|current size||
|H4P_EVENT_Q|Q length has been measured|current size||
|H4P_EVENT_LOOPS|Loops/sec measured|current count||
|H4P_EVENT_PD_ENTER|A device has joined the network|device ID|3|
|H4P_EVENT_PD_LEAVE|A device has left the network|device ID|3|
|H4P_EVENT_MQTT_ERROR|PangolinMQTT internal error|error code|3|
|H4P_EVENT_REBOOT|About to reboot|||
|H4P_EVENT_FACTORY|About to factory reset|||
|H4P_EVENT_CMDREPLY|Result of H4P_EVENT_CMD|Error code 0=OK||
|H4P_EVENT_DLL|Plugin dynamically loaded|shortname|3|
|H4P_EVENT_ON|`h4onof` went ON||3|
|H4P_EVENT_OFF|`h4onof` went OFF||3|
|H4P_EVENT_BACKOFF|webUI Q buffer exceeded||3,4|
|H4P_EVENT_HEARTBEAT|One second has passed|N seconds uptime|3|
|H4P_EVENT_ALL|Include ALL events||5|

Notes:
1. Useful for testing / debugging. If *anything* happens when this is sent, you have a problem!
2. This is the most common type used by loggers
3. Internal use, information only. Do not emit.
4. This is due to the terrible design of AsyncWebserver library: It cannot keep up with the rte of messages being sent, and is almsot certain to crsh uness you stop sending *immediately*
5. Except `H4P_EVENT_HEARTBEAT` - those get really annoying really quickly :smile: . If you *really* want "all" (and you don't) use 0xffffffff

## Handling your own events

If you are interested in a single event, there is a simple function you can call:

```cpp
void h4pOnEvent(H4P_EVENT_TYPE t,H4P_FN_USEREVENT f);
```

Names a function `f` (or defines a lambda) to handle the specific event type `t`. For example:

```cpp
...
void sayGoodbye(const string& msg){ // msg is unused in H4P_EVENT_FACTORY
    Serial.println("Morituri te salutant*");
}
...
h4pOnEvent(H4P_EVENT_FACTORY,sayGoodbye);
```

*Latin for *"We who are about to die, salute you"*

If more than one event type needs to be monitored, you need an [H4P_EventListener](ears.md), but before that you need to know how to combine events.

## Handling Multiple Events

### Combining Events

Multiple Events types are specified at the same time by packing them all into bit-mask. Naming several events then uses the C/C++ language biwise-or operator: `|` (vertical bar)

```cpp
uint32_t myMultipleEventBitmask = H4P_EVENT_REBOOT | H4P_EVENT_FACTORY;
...
// myEventHandler will get called if either H4P_EVENT_REBOOT OR H4P_EVENT_FACTORY occurs
H4P_EventListener ears1(myMultipleEventBitmask,myEventHandler); 
```

### Excluding Events

Again using standard C/C++ language bitwise logic operators, this time `&` and `~` (complement) we can exclude certain events. 
Saying `H4P_EVENT_ALL &~ H4P_EVENT_HEARTBEAT` will handle everything *except* `H4P_EVENT_HEARTBEAT`. The `&~` should be read as "and not"

---

# Event Listeners

## Logging

A Logger is simply a plugin that listens for some event type(s) and then does something useful with them, usually showing, saving, relaying, sending them somewhere.

For example, the ready-made loggers tha come with H4Plugins:

* [H4P_SerialLogger](slog.md) displays the event information on the Serial Monitor.
* [H4P_LocalLogger](llog.md) saves the event to a `log.csv` file in Flash FS
* [H4P_MQTTLogger](mlog.md) publishes the event with a user-chosen topic name to the MQTT Server.
* H4P_HttpMySQLLogger sends events to a remote Webserver for storage in a MySQL database
  
**N.B. H4P_HttpMySQLLogger is experimental and has [its own documentation page](h4mysql.md)**

### Creating Your own logger

Writing your own logger is extremely easy. As mentioned above, all you need to do is implement an event handling function. There is a [Custom Logger example sketch](../examples/LOGGING/H4P_CustomLogger/H4P_CustomLogger.ino) but it is *so* simple, we will look at it in detail here.

First is the custom logger class. We name it "lumberjack" for a bit of fun:

```cpp
class myLogger: public H4Plugin {
        void _handleEvent(H4PID pid,H4P_EVENT_TYPE t,const string& msg) override {
            uint32_t msgvalue=atoi(msg.c_str());
            if(msgvalue%2) Serial.printf("That's odd! Type=%s from %s msg=%s\n",h4pgetEventName(t).c_str(),h4pnames[pid].c_str(),msg.c_str());
        }
    public:
        myLogger(uint32_t filter=H4P_EVENT_ALL): H4Plugin("lumberjack",filter){}
};
```

You could omit the filter parameter and "hard-code" a type or types that you register with the underlying H4Plugin like this:

```cpp
    public:
        myLogger(): H4Plugin("lumberjack",H4P_EVENT_USER | H4P_EVENT_MSG){}
```

But its far more flexible to just pass through whatever the user decides when they instantiate your logger:


```cpp
myLogger im_a_lumberjack_and_im_ok(H4P_EVENT_USER);
```

The event handler is very simple (but unusual and not very useful!). It expects the message to contain an integer value and it ignores everything except the odd values.

This means that unless you can guarantee that any event you listen to contains a single integer in its message - which usually you can't - then you need to be very careful!

If we restrict ourselves in the app to emitting H4P_EVENT_USER messages with an integer in the message, then our example will work.

## A generic Listener

While the above information is good to know, there's a shortcut which does 90% of it for you, the [H4P_EventListener](ears.md) plugin. All you need to do is slot in the actual "meat" of the function to be called whenever any registered event occurs 

```cpp
void myHandler(H4PID pid,H4P_EVENT_TYPE t,const string& msg){ ...do something wonderful...};
...
H4P_EventListener bigBrother( ...events you want ... ,myHandler);
```

---
# Event Emitters

So far everything has been about Listening for events, but how do we actually make them happen? Before we continue, it has to be stressed that ***unless you have declared a listener...*** of some kind before doing any of what follows, ***...you will see nothing***! As a minimum, the [H4P_SerialLogger](slog.md) is suggested, which will send all its output to the serial monitor.

The most fundamental method is to call:


```cpp
void h4psysevent(H4PID pid,H4P_EVENT_TYPE t,const std::string& fmt, Args... args);
```

But this requires you to know...

* A valid `pid` value
* The content and format (if any) of the message expected by the specific H4P_EVENT_TYPE
  
... so there are some shortcuts and easier ways

## Simple message logging

First there is

```cpp
void h4pUserEvent(const char* format,...);
```

Which emits an event of type `H4P_EVENT_USER` with a message made up from `printf`-style format string and optional list of parameters, e.g. `h4pUserEvent("Something %s interesting happened","mildly");` - basically anything you want.

Then there is the even easier `PLOG` macro:

```cpp
PLOG("My sensor value from GPIO %d = %d",myPin,666);
```

The difference between this and `h4pUserEvent` is that `PLOG` emits `H4P_EVENT_MSG` events, which is the "standard" for anything that logs "useful information" re progress / performance / diagnostics etc. More improtanty it can be "compiled out"

If you edit [config.h](../src/config.h) and set:

```cpp
#define H4P_LOG_EVENTS      0
```

The all `PLOG` calls in your code effecitvely disappear, making you code leaner, meaner and a lot less chatty. This is great for "production" but not very helpful while testing and/or ebugging, so until you are 100% happy with your code, leave it set to 1.

## Simple event emitting

You should restrict your emissions to `H4P_EVENT_USER` and `H4P_EVENT_MSG` since these two can (usually) do no harm. Emitting any other event can cause bad things to happen if you do it at the wrong time or get the message contents wrong. But if you feel you *must* then use the `PEVENT` macro, e.g.

```cpp
PEVENT(H4P_EVENT_REBOOT,"Because my counter reached %d",666);
```

Because events usually affect processing and/or lifecycle, unlike `PLOG`, `PEVENT` cannot be "compiled out".

## "Standard" emitters

H4Plugins comes with some pre-defined event emitters which are useful during testing or debugging:

| Plugin | Event | Msg |Note|
| :--- | :--- | :--- | :---: |
|H4P_EmitHeap|H4P_EVENT_HEAP|result of `ESP.getFreeHeap()`||
|H4P_EmitLoopCount|H4P_EVENT_LOOPS|Loops per second|1|
|H4P_EmitQ|H4P_EVENT_Q|Length of H4 Queue||
|H4P_EmitTick|H4P_EVENT_HEARTBEAT|Integer number of seconds uptime||

Notes:
1. Requires reconfiguration of H4 config

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)