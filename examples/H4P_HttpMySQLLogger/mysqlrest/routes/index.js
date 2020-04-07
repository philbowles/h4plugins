const express = require('express');
const fs = require('fs');
const router = express.Router();
const mysqlx = require('@mysql/xdevapi');

router.post('/', function(req, res, next) {
    mysqlx.getSession({
        user: 'phil',
        password: 'XXXXXXXX',
        host: 'localhost',
        port: '33060'
    })
    .then(function (session) {
        var db = session.getSchema('H4');
        let myTable = db.getTable('event');
        let fields=Object.keys(req.body)
        let values=[]
        fields.forEach(element => { values.push(req.body[element]) });
        console.warn(values)
        return myTable.insert(fields).values(values).execute();
    })
    .catch(function (err) { console.warn(err) });
});

function sendFile(res,f){
//    console.warn("send file "+f)
    fs.stat(f,function(err,s) {
        res.append('Content-Length', s.size)
        res.append('Content-Type', 'application/octet-stream')
        res.send(fs.readFileSync(f))
    });
}

function send304(res){
//    console.warn("send 304")
    res.statusCode=304
    res.send()
}

router.get('/update*',function(req,res,next){
    const leeway=30 // leeway between compiling & copying to repo to prevent new boot loop
    const repo=__dirname.replace("routes","repo")
    let params=req.params['0'].split("/")
    // [0]=empty [1]=MCU type [2] = compile date [3] = compile time
    let h=req.headers
    if(h["x-esp8266-mode"]==="sketch") { // decode variant
        switch(params[1]){
            case 'SONOFF_BASIC':
            case 'SONOFF_S20':
            case 'SONOFF_SV':
                bin="itead";
                break;
            case 'WEMOS_D1MINI':
                bin="d1_mini";
                break;
            case 'GENERIC':
                bin="generic";
                break;
            default:
                bin="XXX";
        }
        let rqdate=Date.parse((params[2]+" "+params[3]).replace(/_/g," "))
        let d=new Date(rqdate+(1000*60*leeway)) // 30-minute leeway
        let X=`${repo}/H4.ino.${bin}.bin`
        fs.stat(X,function(err,latest){
            if(!err && (d.toISOString() < latest.mtime.toISOString())) sendFile(res,X)
            else send304(res)
        })
    }
    else {
        let sz=parseInt(h['x-esp8266-chip-size'])/1048576;
        let version=h["x-esp8266-version"];
        let files = fs.readdirSync(repo).filter(fn => fn.endsWith(sz+"M.bin"));
        files.sort().reverse() // latest first
        let latest=files[0].substring(7,12) // chop out version
        if(version < latest) sendFile(res,repo+"/"+files[0])
        else send304(res)
    }
})

module.exports = router;
