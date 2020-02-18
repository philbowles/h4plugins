![H4P Flyer](/assets/MQTTLogo.jpg) 

# MQTT Manager (short name="mqtt")

## Adds MQTT management to H4 Universal Scheduler/Timer. Runs on ESP8266/32

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---

# What does it do?

H4 MQTT Manager controls all aspects of MQTT connection and reconnection after failure. It allows the main body of your code to keep on running if MQTT fails and will automatically re-connect "in the background" when MQTT becomes available again. It also allows "on-the-fly" connection to a different server and maintains a list of all other H4 devices on the local network.

H4 MQTT Manager is a "wrapper" around the well-known and stable "pubSubClient" library, thus all API functions in that library are available to the user as well as several additional H4 functions.

All H4 commands have identical syntax across all H4Plugins. When a command is sent from MQTT, it needs an additional prefix, which controls which of your devices receives the message. 

H4P_MQTT automatically subscribes to some special topics: `all/#`, `< your device name >/#`, `< your board type >/#` and `< your board's unique chip ID >/#`

Assuming the actual command is `h4/reboot` then:

```cpp
all/h4/reboot // reboots every H4 device on your network
WEMOS_D1MINI/h4/reboot // reboots every H4 device running on a Wemos D1 mini board
17D858/h4/reboot // reboots only the device with chip ID of 17D848 *see note
mydevice/h4/reboot // reboots only the device whose local name is "mydevice"
```

Most often you will use the last form to address one specific device.

## (* Do not rely on this form, it is likely to be removed in the next release)

---

# Usage

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS

H4P_WiFi h4wifi(...
H4P_MQTT h4mqtt(...
```

## Dependencies

* [H4P_WiFi](h4wifi.md)

## Commands Added

* h4/show/mqtt
* h4/mqtt/change/w,x,y,z (payload: w=newbroker, x=newport, y=newusername, z=newpassword)
* h4/mqtt/grid (show all local H4 devices)
* h4/mqtt/restart
* h4/mqtt/start
* h4/mqtt/stop

## Topics automatically published

H4P_MQTT publishes `all/h4/mqtt/online` when it connects up and `all/h4/mqtt/online` when it loses the MQTT connection, both with a payload of < your device name > . This allows 3rd party software e.g. NODE-RED as well as H4P_MQTT itself to maintain a list of all devices on the network, providing of course that the current device isn't the one causing the "offline" status :)

This requires that those corresponding commands are publicly exposed / available, but *they should never be invoked by the user*

## Callbacks

```cpp
void onConnect(void);
void onDisconnect(void);
uint32_t onMessage(vector<string>); // where "onMessage" is a user-defined topic handling function
```

## Unloadable

No, but can be stopped with `h4/mqtt/stop` command.

---

# Message handling

To do any useful work at all you *must* implement a function to handle the `onConnect` event, e.g.

```cpp
void onMQTTConnect(){
    // do all your subscribes in here
    h4mqtt.subscribe(.... // standard pubsubclient subscribe
    h4mqtt.subscribeDevice(... // special H4 subscribe (see later)
    ...
}

...
H4P_MQTT h4mqtt("myMQTTserver.local",1883,"admin","admin",onMQTTConnect);

void h4setup(){
    ...
}
```

In that function, (and *nowhere else* ) you must subscribe to any topics your sketch requires. You are strongly advised againt doing any "major work" other than subscribing and to provide a similar function for the `onDisconnect` event which unsubscribes to the same topics. 

Once you have done this you will then also need to provide a "topic handler" function for any of the topics you just subscribed to to. Beause of the way ArduinoIDE works, you will need to define these functions *before* the `onMQTTConnect` function.

## The message itself

As already mentioned you can call any method of the [Arduino pubsubclient library](https://github.com/knolleary/pubsubclient) using e.g. `h4mqtt.subscribe(...`  In this case you will be responsible for defining the callback and parsing (splitting apart and "understanding",validating and responding to the message and the payload. This can be quite tricky if you are new to it and has a number of issues that can cause big problems:

* The message callback is very much like an Interrupt Service Routine ("ISR") and what you can do inside it is very limited.
* Calling publish inside a subscribe callback function is a recipe for disaster
* It goes completely outside H4's "main loop" philosophy and will almost certainly "break" your code unless you are an expert.

For all of the above reasons, it is recommended that you use *only* H4P_MQTT functions. The benefit is they can contain any code you want and as much of it as you want and you can call "publish" with no fear. Also, H4P_MQTT provides a lot of functionality for you, using `subscribeDevice` and `publishDevice`. Unless/until you are confident with coding for MQTT message handling *and* are an expert, stick with the H4P_MQTT way.

## Subscribing

It is important that you understand the MQTT topic syntax before reading his section. If this is not the case, follow this link to [read about MQTT topic syntax](https://mosquitto.org/man/mqtt-7.html) before continuing.

When you call `subscribeDevice` you tell H4P_MQTT the topic and your callback function to handle the topic. In the following examples, let's assume your device is called `mything` and you want it to handle `mytopic`.

The first thing to note is that your device name is added to the front of the topic, so "under the hood" you are actually subscribing to `mything/mytopic`. This allows you to have many devices in the system running the same code, but only have one specific device respond to a given message.

Secondly, the message and payload are parsed and split up for you already and passed to your callback as a `std::vector<std::string>` Don't worry if you don't know what that means, the examples below will show how easy it is to use - essentially it's just like an array of strings. In our example:

```cpp
vs[0]=whatever was in the message payload
```

You may wonder why you get a `vector<string>` when there is only 1 item: the payload, but this will become obvious later when we talk about subtopics and wildcards.

No matter how many parts there are to the message, the payload is always the last item. To make life easier H4 has macros `PAYLOAD` if you are expecting a string and `PAYLOAD_INT` if you are expecting a number. 

Also there is a global string called `H4_MQTT::target` which will contain the message prefix (see above) such as "all" or "mything" or "WEMOS_D1MINI" depending on how/why you received this message. Normally you don't need to know this, but it's there if you want it.

Your callback then "does it thing" but *must* return a value showing if it succeeded or not. It can be any of the following:

```cpp
enum H4_CMD_ERROR:uint32_t  {
    H4_CMD_OK,
    H4_CMD_UNKNOWN,
    H4_CMD_TOO_FEW_PARAMS,
    H4_CMD_TOO_MANY_PARAMS,
    H4_CMD_NOT_NUMERIC,
    H4_CMD_OUT_OF_BOUNDS,
    H4_CMD_NAME_UNKNOWN,
    H4_CMD_PAYLOAD_FORMAT,
    H4_CMD_PROHIBITED
};
```

## Simple example

Pulling together all of the above, the simplest callback will look like something like this:

```cpp
uint32_t myCallback(vector<string> vs){
    Serial.printf("Msg received with payload=%s\n",PAYLOAD.c_str()); // convert payload to C-style string
    if(PAYLOAD=="good") return H4_CMD_OK;
    else return H4_CMD_PAYLOAD_FORMAT;
}
```

...and you would set it up like this:

```cpp
h4mqtt.subscribeDevice("mytopic",myCallback); // MUST be done from inside onConnect callback
```

[Example Code](../examples/H4P_MQTT_Simple/MQTT_Simple.ino)

## Subtopics

You may need to implement multiple topics in a kind of "tree" hierarchy like the following, after all, H4Plugins does exactly this.
* `mytopic/a/b/x`
* `mytopic/a/b/y`
* `mytopic/a/c/x`
* `mytopic/a/d/x`

There are two ways to do it. Both can get quite complicated so its best to stick to simple topics if you can, to start with. In this example we are going to look at subtopics and assume that you want to subscribe to `a/b/c` and then in the next section we will look at "wildcards".

(What follows is a feature of the way H4Plugins builds its internal command table and nothing to do with the way MQTT works)

If you just subscribe to `a/b/c` your code will not get called. You need instead to do the following:

```cpp
void onMQTTConnect(){
    h4mqtt.subscribeDevice("a",myCallback);
    h4mqtt.subscribeDevice("a/b",myCallback);
    h4mqtt.subscribeDevice("a/b/c",myCallback);
}
```
i.e. you need to subscribe to each part of the path in turn. This implies that you will also get any topic `a`, any topic `a/b` as well as the desired `a/b/c` .

You will be able to tell which is which because of the size of the `vector<string>` input, "`vs`". It never includes the first part of the topic because in the simple case there is only one part and you know what it its, or your code would never get to the topic handler in the first place. 

Look at it another way from the code above: all topics starting `a`... get sent to `myCallback` ,so we always know what the first part would be if it were included... so including what we already know just wastes space. Other than this, each subtopic is contained in the next entry in vs, with the payload always being in the final entry.

So if someone publishes `a/b/c` with a payload of "sub-standard", `vs` would look like:

```cpp
vs[0]="b"
vs[1]="c"
vs[2]="sub-standard"
//
vs.size() == 3
```

If the message was `a/b` with a payload of "sub-standard", `vs` would look like:

```cpp
vs[0]="c"
vs[1]="sub-standard"
//
vs.size() == 2
```

...and if someone publishes `a` with a payload of "sub-standard", `vs` would look like:

```cpp
vs[0]="sub-standard"
//
vs.size() == 1
```

As you can see, this could get complicated when multiple subtopics are required, which is why the world invented MQTT wildcards.

[Example Code](../examples/H4MQTT/MQTT_Wildcards/MQTT_Wildcards.ino)

## Wildcard topics

**N.B.** **H4P_MQTT Supports ONLY `#` wildcards. It does NOT support `+` wildcards**

When handling wildcard topics, your callback needs to take care to validate *everything* after the "#" since neither you nor H4P_MQTT - by defintion - can predict what it will be and MQTT itself will allow *anything*. Assume for example you subscribe to `cards/#` and you are only really interested in `cards/hearts`, `cards/clubs`, `cards/diamonds` and `cards/spades`.

You could easily receive `cards/junk/Ace/morejunk/totalrubbish/63/XYZ`. H4P_MQTT will dutifully parse this into:

```cpp
// don't forget the first part is always omitted: this code IS the 'cards' handler!
vs[0]="junk"
vs[1]="Ace"
...
vs[6]=whatever was in the message payload
```

You could also just receive `cards` since ".../#" accepts *anything* after the "/", *including nothing at all* !

If your code expects exactly one subtopic, it will almost certainly break when there are none. If it relies on `vs[0]` being a valid suit, it *will* break. It is up to you to manage this.

You *must* validate both the number of arguments (subtopics) and "sensible" values for each before trying to process the message, remembering to return the appropriate erro code if you are unable to handle the message.

[Example Code](../examples/H4MQTT/MQTT_Wildcards/MQTT_Wildcards.ino)

---

# API

```cpp
// Constructor
H4P_MQTT(string ssid,string psk,string device="",H4_FN_VOID onConnect=[](){},H4_FN_VOID onDisconnect=[](){});
// device is the local name for e.g. OTA. Will be visible in your system as < device >.local
// onConnect = user callback when MQTT connects / reconnects
// onDisconnect = user callback when MQTT disconnects
//
void change(string broker,uint16_t port); // switch to new MQTT broker
void publishDevice(string topic,string payload=""); // publish <device>/topic with string payload
void publishDevice(string topic,uint32_t payload); // publish <device>/topic with numeric payload
void restart(); // stop then start
void showGrid(); // display all local H4 devices
void start();
void stop();
void subscribeDevice(string topic,H4_FN_MSG f); // call f when <device>/topic message received
void unsubscribeDevice(string topic);
```

---

## "Tweakables"

The following values are defined in `H4PConfig.h` . They are chosen initally to set a good balance between stability, performance and memory / stack usage. *It is not advisable to change them unless you know exactly what you are doing and why*. 

**N.B.** **Support will not be provided if any of these values are changed.**

* H4MQ_RETRY            5000

The number of milliseconds H4P_MQTT will wait between attempst to reconnect if server disappears.

* H4MQ_MQTT_RATE        1000

The number of milliseconds for each call of `mqttClient.loop()`. Most basic sketches call this on every loop, which amounts to 40000 - 60000 times *per second* which is a massive peformance hit and totally unnecessary.

The larger the value, the less of a performace hit, BUT the longer it takes for published messages to arrive at the server. If you make the value too large, you cause exponential queue growth if you also publish a lot of messages. If you only publish say, once a minute then you can crank this up...but do not got past 15000 as pubsubclient will time out on its own and close the connection.

Making it smaller will clear the queue and make the mesages arrive at the server more quickly, but - of course - will hurt overal performance / throughput

---

(c) 2020 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Blog](https://8266iot.blogspot.com)
* [Facebook Esparto Support / Discussion](https://www.facebook.com/groups/esparto8266/)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook IOT with ESP8266 (moderator)}](https://www.facebook.com/groups/1591467384241011/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)
