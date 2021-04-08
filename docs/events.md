![H4P Logo](/assets/DiagLogo.jpg)

# Events, Emitters and Listeners

Essential background for getting started with H4Plugins

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
  * [H4P_EmitLoopCount](loops.md)
  * [H4P_EmitQ](eq.md)
  * [H4P_EmitTick](tick.md)

---

# EVERYTHING is an event

*Everything* you are going to deal with in H4Plugins is an event. That's only 99% true, but you will learn about the other 1% as you become more expert with the framework. You should know already from reading the [H4 documentation](https://github.com/philbowles/H4) why we *need* "Event-driven programming" - and hopefully that is why you are here, but what exactly *is* an event? 

In its broadest sense and event is something that happens in the real world, for example:

* User presses a button on a GPIO pin
* Alexa voice control sends a request
* Trying to access a remote server which "times out"

These are the happenings that H4 translates into a list of tasks in a queue, but sometimes things happen so fast that waiting for the next task can be too long: for these things we have events. They are used heavily by the GPIO system for example, where things need to be dealt with very rapidly. H4Plugins can also create its own internal events - these look to your code exactly the same as any other event and there is a single common interface for dealing with all events.

Your code tells H4Plugins which specifc type of events your are interested in and what to do when one happens. H4Plugins does all the "heavy lifting" and your code will look like a big `switch` statement with short, simple sections of your own coide reacting to those events.

## The anatomy of H4Plugins events

If you have used javascript, these concepts should already by familiar to you, but read on to ensure you understand H4Plugins' use of the terminology. Similarly if you are familiar with MQTT, H4Plugins events are much like topics in the publish / subscribe model.

H4Plugins events have 3 pieces of information:

* The "source" of the event (where it came from / what caused it)
* Optionally, some data attached to the event (the "message")
* The type of event

### Event sources

Each plugin is a separate piece of code that runs as a "service" and has a short name associated with it, for example `wifi` or `mqtt`. For this reason it is often referred to as `svc`. You can read more about services here: [Common Command and Control: H4P_SerialCmd and Services](ccc.md). Generally speaking the source will be the short name of the plugin that caused the event. When dealing with GPIO pins, the source is the pin number. During startup before any plugins are running, it will be `h4p`.

### Event messages

The message part of the event can contain any data, depending on the type of message. For example with GPIO it is the new value of the GPIO pin. Even though many events may send numeric values, the message type is always `std::string`. It is easy to call `atoi` to get an integer value if required. Where an event has nothing useful to add, the message will be an empty string, e.g. `msg==""` and `msg.size()==0` are both `true`

### Event types

| Event Name | Meaning | Msg contains | Note |
| :--- | :--- | :--- | :---: |
|H4PE_BOOT|System Start||1|
|H4PE_STAGE2|Sytem Start 2||1|
|H4PE_SERVICE|'svc' goes up/down|1=up, 0=down|1|
|H4PE_REBOOT|About to reboot||2|
|H4PE_FACTORY|About to factory reset||2|
|H4PE_SYSINFO|Important information|Arbitrary message||
|H4PE_SYSWARN|Possible problem - fix it|Arbitrary message||
|H4PE_SYSFATAL|System cannot continue|Arbitrary message|3|
|H4PE_HEAP|Heap has been measured|current size|4|
|H4PE_LOOPS|Loops/sec measured|current count|4|
|H4PE_Q|Q length has been measured|current size|4|
|H4PE_CMD|A command was received|The command||
|H4PE_UIADD|Add web UI item|item parameters|1|
|H4PE_UISYNC|Update web UI|item name|1|
|H4PE_PD_ENTER|A device has joined the network|device ID||
|H4PE_PD_LEAVE|A device has left the network|device ID||
|H4PE_HEARTBEAT|1-second "tick"|formatted uptime||
|H4PE_MSG|Log a message|arbitrary text||
|H4PE_SIGNAL|Flash pattern|timebase,pattern|5|
|H4PE_VIEWERS|Number of actual web users|0=no viewers,1=at least 1||
|H4PE_GPIO|Pin activity|depends on pin||
|H4PE_GVCHANGE|A global variable has changed|new value||

Notes:
1. Internal use, for information only. Do not emit, listen out for etc...
2. Monitor these if you have resources to cleardown / erase
3. This is a radical "fail-safe" state: system will loop indefintely to prevent any GPIO activity.
4. For diagnostic / tuning purposes only
5. See [H4P_Signaller](h4fc.md)

As you can see, many of the evnts are for internal use only and you only need to be aware of them if you are writing your own plugin or doing some of the things discussed in [Advanced Topics](docs/advanced.md). Most users wil only need to react to:

* H4PE_GPIO
* H4PE_GVCHANGE

GPIO messages and the whole GPIO susbsytem are discussed in: [GPIO Handling and "NODE-PINK"](gpio.md). Global variables are discussed in:  [Global Variables and persistent storage](globals.md)

---

# Event Listeners

Technically speaking, an Event Listener is *any* piece of code that registers a handler for a known event and provides a callback function of the form:

```cpp
void MyVeryOwnEventHandler(const string& svc,H4PE_TYPE t,const string& msg){ ... do some interesting stuff }
```

All plugins do this automatically, so all plugins are - by definition - Event Listeners.

## Handling your own events

***IMPORTANT CAVEAT:***

One of the "gotchas" that newcomers to ESP8266 / ESP32 suffer especially if they are moving from say PIC or Arduino or STM32 programming is that *yours is not the only code running*. That's the whole reason you are here, because to get muliple things happening at the same time you have to cope with that and thats why H4 and H4Plugins were written, to make that easy for you.

BUT..."easy" doesn't mean "invisible": you still have to be aware that there is a lot of other code running and it too emits and listens for events. This raises a very a important point:

***All handlers, incuding the ones you write get called for ALL events of that type, whether you sent them or not***. 
(All events are "global" in effect)

This means that you may well have to exmine both the `svc` and/or `msg` fields to decide if it's "one of yours" before acting on any particular event.

With this in mind, if you are interested in a single event, there is a simple function you can call:

```cpp
void h4pOnEvent(H4PE_TYPE t,H4P_FN_USEREVENT f);
```

Names a function `f` (or defines a lambda) of type `H4P_FN_USEREVENT` (whcih just means its a void function with a string parameter) to handle the specific event type `t`. For example:

```cpp
...
void sayGoodbye(const string& msg){ // msg is unused in H4PE_FACTORY
    Serial.println("Morituri te salutant"); // Latin for *"We who are about to die, salute you"*

}
...
h4pOnEvent(H4PE_FACTORY,sayGoodbye);
```

If more than one event type needs to be monitored, you need an [H4P_EventListener](ears.md), but before that you need to know how to combine events.

## Handling Multiple Events

### Combining Events

Multiple Events types are specified at the same time by packing them all into a bit-mask. Naming several events then uses the C/C++ language bitwise-or operator: `|` (vertical bar)

```cpp
uint32_t myMultipleEventBitmask = H4PE_REBOOT | H4PE_FACTORY;
...
// myEventHandler will get called if either H4PE_REBOOT OR H4PE_FACTORY occurs
H4P_EventListener ears1(myMultipleEventBitmask,MyVeryOwnEventHandler); 
```

### Excluding Events

Again using standard C/C++ language bitwise logic operators, this time `&` and `~` (complement) we can exclude certain events. 
Saying `H4PE_ALL &~ H4PE_HEARTBEAT` will handle everything *except* `H4PE_HEARTBEAT`. The `&~` should be read as "and not"

---
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
class myLogger: public H4Service {
        void _handleEvent(const string& svc,H4PE_TYPE t,const string& msg) override {
            uint32_t msgvalue=atoi(msg.c_str());
            if(msgvalue%2) Serial.printf("That's odd! Type=%s from %s msg=%s\n",h4pGetEventName(t).c_str(),svc.c_str(),msg.c_str());
        }
    public:
        myLogger(uint32_t filter=H4PE_ALL): H4Service("lumberjack",filter){}
};
```

You could omit the filter parameter and "hard-code" a type or types that you register with the underlying H4Service like this:

```cpp
    public:
        myLogger(): H4Service("lumberjack",H4PE_USER | H4PE_MSG){}
```

But its far more flexible to just pass through whatever the user decides when they instantiate your logger

The event handler is very simple (as well as unusual and not very useful!). It expects the message to contain an integer value and it ignores everything except the odd values.

This means that unless you can guarantee that any event you listen to contains a single integer in its message - which usually you can't - then you need to be very careful!

If we restrict ourselves in the app to emitting H4PE_MSG messages with an integer in the message, then our example will work, but only as long as no other code sends H4PE_MSG with text values.

## A generic Listener

While the above information is good to know, there's a shortcut which does 90% of it for you, the [H4P_EventListener](ears.md) plugin. All you need to do is slot in the actual "meat" of the function to be called whenever any registered event(s) occur(s) 

```cpp
void myHandler(const string& svc,H4PE_TYPE t,const string& msg){ ...do something wonderful...};
...
H4P_EventListener bigBrother( ...events you want ... ,myHandler);
```

The simplest of example of this is the predefined [H4P_SerialLogger](slog.md) where the "...do something wonderful..." code simply prints the message with `Serial.printf`

## h4pGlobalEventHandler: The Global Listener

Usually most helpful during testing and development (and pretty intrusive and annoying at all other times), you may provide a function called `h4pGlobalEventHandler`. If you do then *every single event* emitted by any code in the system will arrive here, so be prepared. There is no filter, it's *global*, hence the name. You get *everything* whether you want it or not. Probably mostly "not".

```cpp
void h4pGlobalEventHandler(const std::string& svc,H4PE_TYPE t,const std::string& msg){
  Serial.printf("Big Brother sees EVERYTHING %s %s %s\n",svc.c_str(),h4pGetEventName(t).c_str(),msg.c_str());
}
```

---
# Event Emitters

So far everything has been about Listening for events, but how do we actually make them happen ourselves? Before we continue, it has to be stressed that ***unless you have declared a listener...*** of some kind before doing any of what follows, ***...you will see nothing***! As a minimum, the [H4P_SerialLogger](slog.md) is suggested.

The most fundamental method is to call:

```cpp
void h4psysevent(const string& svc,H4PE_TYPE t,const std::string& fmt, Args... args);
```

But this requires you to know...

* A valid `svc` name or value
* The content and format (if any) of the message expected by the specific H4PE_TYPE
  
... so there are some shortcuts and easier ways

## Simple message logging

First there is

```cpp
void h4pUserEvent(const char* format,...);
```

Which emits an event of type `H4PE_MSG` with a message made up from `printf`-style format string and optional list of parameters, e.g. `h4pUserEvent("Something %s interesting happened","mildly");` - basically anything you want.

More importantly it can be "compiled out"

If you edit [plugins_config.h](../src/plugins_config.h) and set:

```cpp
#define H4P_LOG_EVENTS      0
```

Then all `h4pUserEvent` calls in your code effecitvely disappear, making you code leaner, meaner and a lot less chatty. This is great for "production" but not very helpful while testing and/or debugging, so until you are 100% happy with your code, leave it set to 1.

## Simple event emitting

You should restrict your emissions to `H4PE_MSG` since this can (usually) do no harm. Emitting any other event can cause bad things to happen if you do it at the wrong time or get the message contents wrong.
## "Standard" emitters

H4Plugins comes with some pre-defined event emitters which are useful during testing or debugging:

| Plugin | Event | Msg |Note|
| :--- | :--- | :--- | :---: |
|[H4P_EmitHeap](heap.md)|H4PE_HEAP|amount of free heap||
|[H4P_EmitLoopCount](loops)|H4PE_LOOPS|Loops per second|1|
|[H4P_EmitQ](eq.md)|H4PE_Q|Length of H4 Queue||
|[H4P_EmitTick](tick.md)|H4PE_HEARTBEAT|Integer number of seconds uptime||

Notes:
1. Requires reconfiguration of H4 config

---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)