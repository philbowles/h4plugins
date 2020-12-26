![H4P Logo](/assets/DiagLogo.jpg)

# Serial Commands  / General command handling

## Adds external command / control handling to H4 Universal Scheduler/Timer.

*All plugins depend upon the presence of the [H4 library](https://github.com/philbowles/H4), which must be installed first.*

---

# What does it do?

H4P_SerialCmd is the "command and control" centre of H4 and its plugin system. It provides the basis for your app to be controlled by external commands from a variety of different sources such as MQTT, Http/REST or your own code. By default, it adds the serial console as a command source, hence its name. 

H4Plugins have a common command format across all sources, based on MQTT-style topics. While the different sources require slightly different treatment in how the command is entered by the user, the syntax of the command itself is the same in all cases.

Every other plugin has the ability to add its own specific commands on top of those provided by H4P_SerialCmd itself. Details of the additional commands that each plugin adds (if any) are found in the documentation for the relevant plugin.

## Command format

The general concept of the command-line format is designed to emulate MQTT. The major difference is that the sources other than MQTT have no separate field for the payload, so the last item of the command in those cases is treated as the payload. For example, using the Serial monitor:

```cpp
h4/some/cmd/with/many/levels/42,666
```

The payload is "42,666" and the command is handled exactly the same way as an MQTT command with a topic of `h4/some/cmd/with/many/levels` and  payload of "42,666"

This allows a consistent interface for all command handling irrespective of the source. See the [**H4P_AsyncWebServer** (http "REST")](h4asws.md) and [**H4P_AsyncMQTT**](h4mqtt.md) plugins for examples of this in action.

The following external events all cause the same action: rebooting the device

* Typing `h4/reboot` in the Serial monitor
* Typing `http://your.ip.addr.ess/rest/h4/reboot` in a browser
* Publishing `your-device-name/h4/reboot` to the MQTT server
* Calling `h4reboot()` in your code
* Calling `h4cmd.InvokeCmd("h4/reboot")` in your code

## Direct invocation of commands from code

Any command that can be typed on the console, received from MQTT or the http REST interface can be called from your own code using one of two methods:

* InvokeCmd

SerialCmd provides the `invokeCmd` function which takes parameters for topic and payload. Using the above example, you could call it thus:

```cpp
h4cmd.invokeCmd("h4/some/cmd/with/many/levels","42,666");
```

* Direct call

Generally, each plugin also provides functions that correspond to the command-line equivalent, but are more efficient than the invokeCmd method. For example, calling

```cpp
h4cmd.showQ();
```

Has the same effect as typing "h4/show/q" at the console, MQTT publishing a topic of "h4/show/q" or receiving `http://< some ip >/rest/h4/show/q`. The only difference is that the resulting dump of the H4 queue is sent back to the originating source, respectively the console, MQTT server, or the web browser.

---

# Usage

```cpp
#include<H4Plugins.h>
H4_USE_PLUGINS(115200,20,false) // Serial baud rate, Q size, SerialCmd autostop
H4P_SerialCmd h4cmd(... // optional autoStop true/false parameter: true=peformance++ (see later)
```

## Dependencies

none, but when preceded by [H4P_CmdErrors](h4ce.md) numeric codes are translated to meaningful messages

---

# Service control

All H4Plugins act as "services" which means that each plugin can be stopped or started by the user. In some cases this can have a major effect on how your app behaves, so use these with caution and only after you fully understand how each plugin works.

In all cases, the payload `pppp` is the name of the plugin

* h4/svc/restart/pppp // ...stop/ppp followed by .../start/pppp
* h4/svc/start/pppp 
* h4/svc/state/pppp // shows different information relating to each specific plugin (previously was h4/show/xxxx )
* h4/svc/stop/pppp

## Commands Added by SerialCmd

* h4/dump/x (payload x = SPIFFS file name. Show content of file.) 
* h4/factory
* h4/reboot
* h4/show/all
* h4/show/config
* h4/show/heap
* h4/show/plugins
* h4/show/q
* h4/show/spif  (show all SPIFFS files + sizes and used / free space)
* help

---

### Error Messages

Validation of commands is fairly basic, to reduce code size. Two main principles underlie command handling:

* Silent failure ("do no harm")

If a command is valid, but is not a sensible thing to do at the time then it simply does nothing and no error message is received (the command was syntactically valid, after all).

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

[Example Code](../examples/BASICS/H4P_SerialCmd/H4P_SerialCmd.ino)

---

# Service Commands

* h4/svc/restart/scmd // no effect
* h4/svc/start/scmd // no effect
* h4/svc/state/scmd 
* h4/svc/stop/scmd 
* 
SerialCmd can be safely stopped - it will continue to provide command functionality to other plugins that depend upon it. e.g. [**H4P_AsyncWebServer** (http "REST")](h4asws.md) and [**H4P_AsyncMQTT**](h4mqtt.md) but it will no longer accept serial commands on its own behalf. Thus it can only be done once from the serial console*. Unloading `scmd` will show a huge performance improvement (+50% ish), so it is worth considering once testing is complete, especially for devices that will be deployed remotely and will never be able to recive serial commands. (see autoStop parameter below)

(* It can be restarted remotely, e.g. via MQTT or http/REST)

---

# API

## Global Callbacks

```cpp
void onFactoryReset(void);
void onReboot(void);
```

# API

```cpp
/* Constructor
autoStop = true will stop the servce form receiving serial commnds and improve performance by about +50%
*/
H4P_SerialCmd(bool autoStop=true);
```

## Command functions

```cpp
void all();
void config();
void showQ();
void h4reboot(); //** provided by h4, use h4.h4reboot(), NOT h4cmd.h4reboot()
void h4FactoryReset();
void help();
void plugins();
void showSPIFFS();
```

### all()

#### Equivalent command-line: h4/show/all

Runs all commands from any loaded plugins containing "show", i.e. `h4/show/q` ... etc and also runs `h4/svc/state/...` on all loaded plugins

### config()

#### Equivalent command-line: h4/show/config

Shows the contents of H4's internal configuration items. Useful for debugging, but will probably only be relevant to expert users.

### showQ()

#### Equivalent command-line: h4/show/q

Shows the contents of H4's internal queue. Useful for debugging, but will probably only be relevant to expert users.

### h4FactoryReset()

#### Equivalent command-line: h4/factory

```cpp
h4FactoryReset();
```

i.e. *do not* precede it with `h4cmd.`

**WARNING** *this will delete all saved configuration data before doing a reboot - use with caution!*

### h4reboot()

#### Equivalent command-line: h4/reboot

Does exactly what it says on the tin! **NOTE** This command is provided by H4 itself, not SerialCmd so it is available even if SerialCmd is not loaded. It is included here because of SerialCmd's h4/reboot command. If calling it directly, the correct form is:

```cpp
h4reboot();
```

i.e. *do not* precede it with `h4cmd.`

### help()

#### Equivalent command-line: help

Shows all commands available across all installed plugins

### plugins()

#### Equivalent command-line: h4/show/plugins

Shows a list of all currently loaded plugins/service

Some plugins depend on others ( see for example [**H4P_CmdErrors**](h4ce.md) ), The plugin shortname is the identifier that plugins use to see which other plugins are installed.

---

## General purpose functions

```cpp
uint32_t invokeCmd(string,string="",const char* src="user");
uint32_t invokeCmd(string,uint32_t,const char* src="user"); // for single integer payloads
Both variants return the error code - if any - or zero on success. (see below)
void logEventType(H4P_LOG_TYPE,const string& src,const string& tgt,const string& fmt,...);
// see Logging documentation https://github.com/philbowles/h4plugins/blob/master/docs/h4slogs.md
//          SPIFFS
string read(const string& fn); // reads contents of SPIFFS file into string
uint32_t write(const string& fn,const string& data,const char* mode="w"); // "w"rites or "a"ppends string to SPIFFS file, return file size.
//      advanced (separate documentation T.B.A)
void addCmd(const string& name,uint32_t owner, uint32_t levID,H4_FN_MSG f=nullptr);
void removeCmd(const string& name); 

```

---

(c) 2020 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Blog](https://8266iot.blogspot.com)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook IOT with ESP8266 (moderator)}](https://www.facebook.com/groups/1591467384241011/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)
