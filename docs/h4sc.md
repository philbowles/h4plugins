![H4P Logo](/assets/H4Plogo.png)

# Serial Commands

## Adds external control handling to H4 Universal Scheduler/Timer.

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---

# What does it do?

Provides the basis for H4 Plugins to be controlled from the serial console. Each plugin has the ability to add its own commands so you only see those that are provided by the currently used set of plugins.

Details of the commands that each plugin adds (if any) are found in the documentation for the relevant plugin.

H4Plugins has at is core a command processor allowing user commands to come form several sources:

* Serial Console (when using this plugin)
* HTTP REST API (when using the [H4P_WiFi](h4wifi.md) plugin)
* MQTT (when using the [H4P_MQTT](h4mqtt.md) plugin)
* User code by calling API functions directly

While the different sources require slightly different treatment, the syntax of the command itself is common across all sources.

## Command format

The general concept of the command-line format is designed to emulate MQTT. The major difference is that there is no separate field for the payload, so the last item of the command line is treated as a payload. For example, using the Serial monitor:

```cpp
h4/some/cmd/with/many/levels/42,666
```

The payload is "42,666" and the command is handled exactly the same way as an MQTT command with a topic of `h4/some/cmd/with/many/levels` and  payload of "42,666"

The rationale behind this is that there is a consistent interface for all command handling irrespective of the source. See the [**H4P_AsyncWebServer** (http "REST")](h4asws.md) and [**H4P_MQTT**](h4mqtt.md) plugins for examples of this in action.

So the following event all cause the same result: rebooting the device

* Typing `h4/reboot` in the Serial monitor
* Typing `http://your.ip.addr.ess/rest/h4/reboot` in a browser
* Publishing `your-device-name/h4/reboot` to the MQTT server
* Calling `h4reboot()` in your code
* Calling `h4sc.InvokeCmd("h4/reboot")` in your code

## Direct invocation of commands from code

Any command that can be typed on the console, received from MQTT or the http REST interface can be called from your own code using one of two methods:

* InvokeCmd

SerialCmd provides the `invokeCmd` function which takes parameters for topic and payload. Using the above example, you could call it thus:

```cpp
h4sc.invokeCmd("h4/some/cmd/with/many/levels","42,666");
```

* Direct call

Generally, each plugin also provides functions that correspond to the command-line equivalent that are more efficient than the invokeCmd method. For example, calling

```cpp
h4sc.dumpQ();
```

Has the same effect as typing "h4/show/q" at the console, MQTT publishing a topic of "h4/show/Q" or receiving `http://< some ip >/rest/h4/show/q`. The only difference is that the resulting dump of the H4 queue is sent back to the originating source, respectively the console, MQTT server, or the web browser.

---

# Usage

```cpp
#include <H4Plugins.h>
H4P_SerialCmd h4sc;
```

## Dependencies

none

## Commands Added

* h4/reboot
* h4/show/q
* h4/show/config
* h4/show/qstats
* h4/show/all
* h4/show/tnames
* h4/show/unload
* h4/unload
* help

## Trusted Name

*SCMD*

## Unloadable

YES: Serial commands will be no longer processed. Other command handling services remain availble to dependent plugins. Overall performance is greatly increased.

---

# API

When calling any function, it must be prefixed by `h4sc.` e.g. `h4sc.dumpQ();` unless otherwise stated.

## General purpose functions

```cpp
uint32_t        invokeCmd(string,string="",const char* src="user");
uint32_t        invokeCmd(string,uint32_t,const char* src="user"); // for single integer payloads
```

Both variants return the error code - if any - or zero on success. (see below)

## Command functions

```cpp
void all();
void config();
void dumpQ();
void h4reboot(); //** provided by h4, use h4.h4reboot(), NOT h4sc.h4reboot()
void help();
void Qstats();
void tnames();
void showUnload();
void unload(const char* pid);
void unload(string pid)
```

### all

#### Equivalent command-line: h4/show/all

Runs all commands containing "show", i.e.

* h4/show/q
* h4/show/qstats
* h4/show/all
* h4/show/tnames
* h4/show/unload
... etc

### config

#### Equivalent command-line: h4/show/config

Shows the contents of H4's internal configuration items. Useful for debugging, but will probably only be relevant to expert users.

### dumpQ

#### Equivalent command-line: h4/show/Q

Shows the contents of H4's internal queue. Useful for debugging, but will probably only be relevant to expert users.

### h4reboot

#### Equivalent command-line: h4/reboot

Does exactly what it says on the tin! **NOTE** This command is provided by H4 itself, not SerialCmd so it is available even if SerialCmd is not loaded. It is included here because of SerialCmd's h4/reboot command. If calling it directly, the correct form is:

```cpp
h4reboot();
```

### help

#### Equivalent command-line: help

Shows all commands available across all installed plugins

### qstats

#### Equivalent command-line: h4/show/qstats

Shows the capacity and current size of H4's internal queue. Useful for debugging, but will probably only be relevant to expert users.

### tnames

#### Equivalent command-line: h4/show/tnames

Shows H4's list of "trusted names". Useful for debugging, but will probably only be relevant to expert users. Each plugins registers at least one "trusted name" when it starts. Since some plugins depend on others ( see for example [**H4P_CmdErrors**](h4ce.md) ), this is a method to see which plugins are installed. It is also the name to be used when calling unload. **WARNING** Do *NOT* call `unload` unless you know what you are doing - it will almost certainly break your system!

### showUnload

#### Equivalent command-line: h4/show/unload

Shows which plugins can be unloaded. SerialCmd itsef can be (see `unload`) using the trusted name "SCMD". Useful for debugging, but will probably only be relevant to expert users.

### unload

#### Equivalent command-line: h4/show/unload/< trusted name >

**WARNING** Do *NOT* call `unload` unless you know what you are doing - it will almost certainly break your system!

Many plugins "hook in" to H4's main loop. This takes processing power away from other features. SerialCmd itself has to check on every loop if there is a command waiting and that can significantly hurt the overall performance. Unloading a plugin simply "unhooks" it from H4's main loop, by defintion preventing it from working which is why it is very dangerous to call as other plugins may break if they depend on the one you just unloaded.

SerialCmd can be safely unloaded - it will continue to provide command functionality to other plugins that depend upon it. e.g. [**H4P_AsyncWebServer** (http "REST")](h4asws.md) and [**H4P_MQTT**](h4mqtt.md) but it will no longer accept serial commands on its own behalf. Thus it can only be done once and cannot be undone. Unloading SCMD will show a huge performance improvement, so it it worth considering once testing is complete, especially for devices that will be deployed remotely.and will never be able to recive serial commands.

### Error Messages

Validation of commands is fairly basic, to reduce code size. Two main principles underlie command handling:

* Silent failure ("do no harm")

If a command is valid, but is not a sensible thing to do at the time, e.g. unloading a trusted name that has already been unloaded then it simply does nothing and no error message is received (the command was syntactically valid, after all).

So while there is little indication why something "hasn't worked" it is because it *never could have worked*, but at least no harm is done.

* Parameter "clipping"

If a numeric value is given to a command but it is too large or too small, e.g. 150% then the value will be "clipped" to within a safe range and the command will run with the clipped value. In the above example it would be as if you had typed 100%, the "sensible" maximum.

If there is no "sensible" alternative then the "Silent failure" rule applies.

#### Error Codes

Meaningful error messages will only be shown if the [**H4P_CmdErrors**](h4ce.md) plugin is also loaded. (If it is, then it must be loaded *before* SerialCmd)
Otherwise a simple code is returned:

```cpp
{0,"OK"},
{1,"Unknown cmd"},
{2,"Too few parameters"},
{3,"Too many parameters"},
{4,"Numeric value expected"},
{5,"Value out of range"},
{6,"Name not known"},
{7,"Incorrect Payload Format"},
{8,"Prohibited from here"}
```

[Example Code](../examples/H4P_SerialCmd/H4P_SerialCmd.ino)

----

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
