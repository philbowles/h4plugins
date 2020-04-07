![H4P Flyer](/assets/DiagLogo.jpg) 

# Advanced Topics

## Deploying multiple devices


### Device naming

If no device name is given in the constructor, it defaults to H4_XXXXXX where XXXXXX is the unique chip ID of the device (usually the last 6 characters of the MAC address).

This is useful to enable a single generic sketch to be uploaded to numerous devices without change. Each device should then be sent a `h4/wifi/host` command to give it a "sensible" name. This can be done by any MQTT client using stored messages (or e.g. NODE-RED), so that each device gets its own new name every time it reboots.

#### Precedence

The situation is a little different if the device has a name defined in the constructor. Assume this is "firstname". When given a `h4/wifi/host` command with a payload of "secondname", it will reboot as - no surprise - "secondname.local".

By default, it will stay as "secondname.local" until the next factory reset. After that it will return again to "firstname" until either another host command changes it, or a new value is compiled in.

## NODE-RED H4 Updater flow

```javscript
[
    {
        "id": "4f02ee37.9678e",
        "type": "tab",
        "label": "H4 Updater",
        "disabled": false,
        "info": ""
    },
    {
        "id": "1db3e693.db6109",
        "type": "http in",
        "z": "4f02ee37.9678e",
        "name": "/update/:type/:date/:time",
        "url": "/update/:type/:date/:time",
        "method": "get",
        "upload": false,
        "swaggerDoc": "",
        "x": 255.5,
        "y": 145,
        "wires": [
            [
                "a9253f25.6403a"
            ]
        ]
    },
    {
        "id": "a9253f25.6403a",
        "type": "function",
        "z": "4f02ee37.9678e",
        "name": "Check Latest Version",
        "func": "const repo=\"/home/pi/H4firmware\"\nconst leeway=30 // leeway between compiling & copying to repo to prevent new boot loop\nlet type=msg.req.params.type\nlet rqdate=Date.parse((msg.req.params.date+\" \"+msg.req.params.time).replace(/_/g,\" \"))\nlet d=new Date(rqdate+(1000*60*leeway)) // 30-minute leeway\nlet h=msg.req.headers\nif(h[\"x-esp8266-mode\"]===\"sketch\") {\n    switch(type){\n        case 'SONOFF_BASIC':\n        case 'SONOFF_S20':\n        case 'SONOFF_SV':\n            bin=\"itead\";\n            break;\n        case 'WEMOS_D1MINI':\n            bin=\"d1_mini\";\n            break;\n        case 'GENERIC':\n            bin=\"generic\";\n            break;\n        default:\n            bin=\"XXX\";\n    }\n    let X=`${repo}/H4.ino.${bin}.bin`\n    context.global.fs.stat(X,function(err,latest){\n        if(!err && (d.toISOString() < latest.mtime.toISOString())) msg.payload=context.global.fs.readFileSync(X)\n        else msg.statusCode=304;\n        node.send(msg)\n        node.done()\n    })\n}\nelse {\n    let sz=parseInt(h['x-esp8266-chip-size'])/1048576;\n    let version=h[\"x-esp8266-version\"];\n    let files = context.global.fs.readdirSync(repo).filter(fn => fn.endsWith(sz+\"M.bin\"));\n    files.sort().reverse() // latest first\n    let latest=files[0].substring(7,12) // chop out version\n    if(version < latest) msg.payload=context.global.fs.readFileSync(repo+\"/\"+files[0])\n    else msg.statusCode=304\n    return msg\n}\n",
        "outputs": 1,
        "noerr": 0,
        "x": 488,
        "y": 145,
        "wires": [
            [
                "55c0bb2a.880414"
            ]
        ]
    },
    {
        "id": "55c0bb2a.880414",
        "type": "http response",
        "z": "4f02ee37.9678e",
        "name": "Send HTTP Reply",
        "x": 704,
        "y": 146,
        "wires": []
    },
    {
        "id": "813f830c.31324",
        "type": "comment",
        "z": "4f02ee37.9678e",
        "name": "Incoming ESPhttpUpdate",
        "info": "",
        "x": 244.5,
        "y": 103,
        "wires": []
    },
    {
        "id": "470f409.fd83cc",
        "type": "comment",
        "z": "4f02ee37.9678e",
        "name": "Is Vn(incoming) < Vn(repo) ?",
        "info": "",
        "x": 493.5,
        "y": 192,
        "wires": []
    },
    {
        "id": "51636172.da02f",
        "type": "comment",
        "z": "4f02ee37.9678e",
        "name": "If so, send binary, else return 304",
        "info": "",
        "x": 739.5,
        "y": 104,
        "wires": []
    }
]
```
