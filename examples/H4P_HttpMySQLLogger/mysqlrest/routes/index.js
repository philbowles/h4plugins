const express = require('express');
const router = express.Router();
const mysql = require('mysql');

const con = mysql.createConnection({
  host: "localhost",
  user: "xxxx",
  password: "xxxx",
  database: "h4"
});

function runSQL(res,sql,send=true){
  console.log("SQL ",sql);
  con.query(sql, function (error, results, fields) {
    if(error) throw error;
    if(send) res.send(results);
  });
}

router.post('/', function(req, res, next) {

    const fields=Object.keys(req.body);
    let ins="";
    fields.forEach((v) => { ins+=v+'="'+req.body[v]+'", '; });
    let fv=ins.slice(0, -2);
    let sql="insert into event set "+fv
    runSQL(res,sql);
});

module.exports = router;
