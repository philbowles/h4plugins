const express = require('express');
const router = express.Router();
const mysql = require('mysql');
/* db schema
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
*/
const con = mysql.createConnection({
  host: "localhost",
  user: "phil",
  password: "CanonD60",
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
