var express = require('express');
var app = express();

// You register your page handlers here:
app.get('/', function(req, res) {
  console.log("Front page request came in here.");
  res.send('This is the front page.');
});
app.get('/simple/:user', function(req, res) {
  var user=req.params.user;
  res.send('Hello there '+user+'!');
});

app.listen(8100, function() {
  console.log('Simple app listening');
});

