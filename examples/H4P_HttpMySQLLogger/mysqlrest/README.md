![H4P Logo](/assets/DiagLogo.jpg)

# H4Plugins NodeJS / express server for MySQL logger

## Requires the installation of

* [H4 library](https://github.com/philbowles/H4)
* [H4Plugins library](https://github.com/philbowles/h4plugins)

---
Version **0.0.1**

## What does it do?

The code opens an http port on 8266 and services POST requests in application/x-www-form-urlencoded format. The body must contain data for the following fields

* type 
* error
* source
* target
* msg 

These are populated automatically by the H4Plugins MySQLlogger

## Prerequites
 
A server running

* MySQL configured for remote access
* NodeJS 
* express

## Installation

Installation of a suitable OS and the above packages is beyond the scope of this document.

1. Create a database in MySQL (assumed to be named h4)
2. Create the following schema

```cpp
use h4;
CREATE TABLE `event` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `type` int(11) NOT NULL,
  `error` int(11) NOT NULL,
  `source` varchar(16) DEFAULT NULL,
  `target` varchar(16) DEFAULT NULL,
  `msg` varchar(64) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id_UNIQUE` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
```

3. Edit routes/index.js and replace the XXXXs with your MySQL username and password

```cpp
  user: "XXXX",
  password: "XXXX",
```
4. Ensure your firewall allows inbound HTTP on port 8266
5. Start the server with `node app`
6. Run the [H4Plugins example H4PHttpLogger](tba) after editing the url with the domain name or IP address of your server 
   
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
