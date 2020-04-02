![H4P Flyer](/assets/DiagLogo.jpg) 

# Advanced Topics

### Device naming

If no device name is given in the constructor, it defaults to H4_XXXXXX where XXXXXX is the unique chip ID of the device (usually the last 6 characters of the MAC address).

This is useful to enable a single generic sketch to be uploaded to numerous devices without change. Each device should then be sent a `h4/wifi/host` command to give it a "sensible" name. This can be done by any MQTT client using stored messages (or e.g. NODE-RED), so that each device gets its own new name every time it reboots.

#### Precedence

The situation is a little different if the device has a name defined in the constructor. Assume this is "firstname". When given a `h4/wifi/host` command with a payload of "secondname", it will reboot as - no surprise - "secondname.local".

By default, it will stay as "secondname.local" until the next factory reset. After that it will return again to "firstname" until either another host command changes it, or a new value is compiled in.

In some circumstances, you may want it to always revert to the compiled in name "firstname" evenafter a host command. The default behavious described aboce cna be changed by editing `H4PConfig.h` and setting `H4P_PREFER_PERSISTENT` to `false`.

