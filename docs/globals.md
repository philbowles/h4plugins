![H4P Logo](../assets/DiagLogo.jpg)
# Global Variables and persistent storage

:gem: Essential background for getting started with H4Plugins

---

# Contents


---

# Introduction

Imagine that instead of a standard C++ `int` or `std::string` data type, you had a "smart" type which could:

* Save its value across reboots ("persistent storage")
* Tell you when its value changes

H4Plugins provides both of these features with its "global variables" concept. The full API is described in [H4P_SerialCmd](h4p.md) but here we explain the basic concepts and some caveats so that you fully understand the API when you come to use it.

---

# Basic concepts

In order for your code to react to different variables changing at different times, each needs a name. Also, it can be useful to have global variables that only exists for the lifetime of this bootup and therefore do not need to be saved across reboots.

## Flash wear: minimise your persistence

Globals are saved in a file in the program Flash partition. This has a finite number of read / write cycles (usually in the tens of thousands) before it can start to fail, so ***it is absolutely essential*** to minimise both the number of persistent global variables you use and how often they get updated.

In order not to lose any persistent values if there is a crash, the globals file is written every time any value in it changes, so makes sure to only change you globals when you cannot avoid it and ony when they are absolutely fundamental to the functioning of your app across reboots.

## Notification of changed values: the H4PE_GVCHANGE event

(You need to have already read and understood [Everything is an event: Listeners, Emitters and Handlers](events.md) ) before continuing

When *any* change is made to a global, no matter where / how / by whom, an `H4PE_GVCHANGE` event is emitted with the `svc` field set to the name of the variable and the `msg` field set to is new value.

## Choosing between int and string

Internally, all globals are stored as `std::string`. If you only ever use them to hold integer values then there are some handy shortcuts (e.g. `inc` and `dec` functions) but all `int`-type globals are converted internally to  `std::string` before being saved, and back again to `int` on retrieval / access.

Therefore, choose the type which reduces the amount of data conversion required. For example if you are only ever holding 1 or 0 and treating the global like you would a standard C++ `bool` then you may as well keep it as a `std::string` and test for "0" or "1" rather than converting it to `int` and testing for 0 or 1. If you need to do maths on it then probably an `int` will be more efficient.

---

# Lifecyle

## Creation

Use either:

```cpp
void gvSetInt(const string& name,int value,bool save=false);
void gvSetstring(const string& name,const string& value,bool save=false);
...
// Examples
h4p.gvSetInt("MyCounter",42,true); // persist across boots
h4p.gvSetString("MOTD","Isn't H4Plugins wonderful? :)",false); // do NOT persist across boots
```

### Naming

H4Plugins uses many of its own globals. To see a complete list, run the command `h4/show/globals` (see [Common Command and Control: H4P_SerialCmd and Services](ccc.md) for what this means and the many ways to do it). Obviously you should not use any of these names!

We suggest you invent a prefix e.g. "My" or your App name - if it is short - and name your own variables in a way that will not clash with either H4Plugins or any other code if other people might later use your work.

If in doubt, use:

```cpp
bool gvExists(const string& name);
```

## Modification

There is a quick and easy way to both set and get an existing value using square bracket notation:

```cpp
h4p["MyCounter"]=666;
std::string MessageOfTheDay=h4p["MOTD"];
```

To retrieve a value you can also use:

```cpp
int gvGetInt(const string& name);
string gvGetstring(const string& name);
// Examples
int mycount =h4p.gvGetInt("MyCounter"); // mycount now = 666
std::string MessageOfTheDay=h4p.gvGetString("MOTD"); // MessageOfTheDay = "Isn't H4Plugins wonderful? :)" (Answer: YES!)
```

For `int` variants, also avilable are:

```cpp
void gvDec(const string& name); // decrement
void gvInc(const string& name); // increment
// Examples
h4p.gvInc("MyCounter"); // mycount now = 667
```

The whole point of any of these is that when they happen, an `H4PE_GVCHANGE` event is emitted with the `svc` field set to the name of the variable and the `msg` field set to is new value. See below for a quick example of handling the event.

## Deletion

When we say "deletion" we mean *completely*: not only will the variable cease to exists in this bootup, it will be deleted from persistent storage and will no longer exist at *next* boot, so take care.

```cpp
void gvErase(const string& name); // remove a single named variable
void gvErase(initializer_list<const char*> nil); // remove sveral at a time using C++ initialiser list
// Examples
h4p.gvErase({"MyCounter","MOTD"}); // 2x for the price of one
```

---

# Handling H4PE_GVCHANGE events

Before we dive in, a reminder of the important caveat in [Everything is an event: Listeners, Emitters and Handlers](events.md#handling-your-own-events) which is worth repeating here because this is exactly where it will "bite" you (hard) if you ignore it:

Your handler will get ***ALL H4PE_GVCHANGE events whether they relate to your variables or not***

This means you will have to "filter out" your own before acting on anything. This is not big deal since your own code was going to have to have separate sections to handle the change of each variable anyway...

```cpp
H4P_EventListener myglobals(H4PE_GVCHANGE,[](const string& svc,H4PE_TYPE t,const string& msg){
    if(svc=="MyCount"){
        // do some stuff with new count
        if(atoi(msg.c_str)) > 1000) ... etc
        return;
    }
    if(svc=="MOTD"){
        Serial.printf("We have a new Message of the day: %s\n",msg.c_str());
        return;
    }
    Serial.printf("Oooh, someone ELSE's variable %s changed to %s\n",svc.c_str(),msg.c_str());    
    Serial.printf("Best leave it alone, eh?\n");    
});
```

Imagine instead that - like me - you are lazy and think: "I only have one global, I know its name - obviously - so no need to check it, just react to the event" and you then write something like this:

```cpp
H4P_EventListener myglobals(H4PE_GVCHANGE,[](const string& svc,H4PE_TYPE t,const string& msg){
    if(atoi(msg.c_str)) == 0) Serial.printf("Blast Off!\n");
});
```

Then you are going to get very confusing results! Firstly, you are probably going to get called dozens of times without your own variable's value having changed! Second, *if* the msg contains what *could look like* an integer value then you will get confusing results. Most likely it will be text in which case `atoi` will return zero and you code will - wrongly - print `Blast Off!`.

*Always check for your own expected variables by name and ignore all others*

***YOU HAVE BEEN WARNED!***
---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)