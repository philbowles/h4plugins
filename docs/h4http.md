![H4P Flyer](../assets/WiFiLogo.jpg) 

# H4P_AsyncHTTP

## Service shortname http

Retrieves data asynchronously from remote http server - a thin wrapper around the [ArmadilloHTTP](https://github.com/philbowles/ArmadilloHTTP) to which the user should also refer

---

## Contents

* [Usage](#usage)
* [Dependencies](#dependencies)
* [Commands Added](#commands-added)
* [Service Commands](#service-commands)
* [API](#api)

---
# Usage

```cpp
H4P_WiFi h4wifi(...
H4P_AsyncHTTP ah;
```

This plugin is a "singleton" - there may be only one single instance of it in the app. 
It may be instantiated as any name the user chooses, prefix all API calls below with that name.

## Dependencies

* [H4P_WiFi](h4wifi.md)

## Commands Added

N/A

# Service Commands

`stop` will prevent remote access any requetst will be silently ignored

# Callbacks

```cpp
ARMA_FN_HTTP rx(ARMA_HTTP_REPLY r); // RX function called when data is received from remote server
// ARMA_HTTP_REPLY is a structure holding:
struct ArmadilloHTTPresponse {
    uint32_t        httpResponseCode;
    VARK_NVP_MAP    responseHeaders;
    ARMA_METHODS    allowedMethods;
    const uint8_t*  data;
    size_t          length;

    std::string     asJsonstring(){ return (responseHeaders[contentTypeTag()].find("json")!=std::string::npos) ? string((const char*) data, length):""; }
    VARK_NVP_MAP    asSimpleJson(){ return json2nvp(asJsonstring()); }
    std::string     asStdstring(){ return string((const char*) data, length); }
};
```

---

# API

```cpp
/*
Constructor
*/
H4P_AsyncHTTP();
//
// common parameters:
// url remote server format "http://host:port/path/?a=b..." can start https://. port, path, query all optional
// rx onData receive function
// fingerprint=nullptr pnly reuired if url starts https://
// phase=ARMA_PHASE_EXECUTE see documentation for ArmadilloHTTP
//
void    DELETE(const std::string& url,ARMA_FN_HTTP rx,const uint8_t* fingerprint=nullptr,uint32_t phase=ARMA_PHASE_EXECUTE) override;
void    GET(const std::string& url,ARMA_FN_HTTP rx,const uint8_t* fingerprint=nullptr,uint32_t phase=ARMA_PHASE_EXECUTE) override;
void    PATCH(const std::string& url,const VARK_NVP_MAP& fields,ARMA_FN_HTTP rx,const uint8_t* fingerprint=nullptr,uint32_t phase=ARMA_PHASE_EXECUTE) override;
void    POST(const std::string& url,const VARK_NVP_MAP& fields,ARMA_FN_HTTP rx,const uint8_t* fingerprint=nullptr,uint32_t phase=ARMA_PHASE_EXECUTE) override;
void    PUT(const std::string& url,const VARK_NVP_MAP& fields,ARMA_FN_HTTP rx,const uint8_t* fingerprint=nullptr,uint32_t phase=ARMA_PHASE_EXECUTE) override;
void    globalsFromSimpleJson(ARMA_HTTP_REPLY r); // creat h4p[] globals from simple json string. each is prefixed by "usr_" see webUI docs
//
```

## Example sketch

* [Simple Remote HTTP](../examples/06_WEB_UI/AsyncHTTP/AsyncHTTP.ino)
* [Pull remote data into webUI](../examples/06_WEB_UI/WebUI_RemoteInclusions/WebUI_RemoteInclusions.ino)
* [Remote Logger](../examples/02_LOGGING/H4P_RemoteLogger/H4P_RemoteLogger.ino)
  
---

(c) 2021 Phil Bowles h4plugins@gmail.com

* [Youtube channel (instructional videos)](https://www.youtube.com/channel/UCYi-Ko76_3p9hBUtleZRY6g)
* [Facebook H4  Support / Discussion](https://www.facebook.com/groups/444344099599131/)
* [Facebook General ESP8266 / ESP32](https://www.facebook.com/groups/2125820374390340/)
* [Facebook ESP8266 Programming Questions](https://www.facebook.com/groups/esp8266questions/)
* [Facebook ESP Developers (moderator)](https://www.facebook.com/groups/ESP8266/)
* [Support me on Patreon](https://patreon.com/esparto)