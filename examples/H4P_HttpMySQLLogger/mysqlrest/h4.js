const port = 8266;
const express = require('express');
const path = require('path');
const indexRouter = require('./routes/index');
const cors = require('cors');

var app = express();
app.use(cors());
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use('/', indexRouter);

// catch 404 and forward to error handler
app.use(function(req, res, next) {  next(createError(404)); });

// error handler
app.use(function(err, req, res, next) {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get('env') === 'development' ? err : {};

  // render the error page
  res.status(err.status || 500);
  res.send('error '+err.status);
});
app.listen(port,()=> console.log("H4plugins listening on "+port))
module.exports = app;