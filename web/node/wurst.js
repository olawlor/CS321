/*
  The Wurst web framework ever: stores JavaScript in a MongoDB database,
  and lets anybody go and edit the JavaScript code, 
  which then runs on the server in 'safe-eval' (which probably isn't all that safe).

  Needs several fancy packages, so:
    npm -g install express escape-html safe-eval request body-parser mongodb

  Full documentation is at:
    https://docs.google.com/document/d/1yocQQouVikESA2X1ASDY88jyupF7j5nit3quEsdXwww/
*/
var express = require('express');

var escape = require('escape-html');
var pre=function(text) {
	return "<pre>"+escape(text)+"</pre>";
}


var safeEval = require('safe-eval');
var request = require('request');
var app = express();
var bodyParser = require('body-parser')
// parse POST data
app.use(bodyParser.urlencoded({ extended: false }))



var MongoClient = require('mongodb').MongoClient;
var url = "mongodb://localhost:27017/";

// The whole application might touch the database, 
//   so we connect before doing anything.
MongoClient.connect(url, function(err, db) {
  if (err) throw err;
  var dbo = db.db("mydb");


// Return the saved code string for this user.
var getCode=function(user,name,callback) {
  dbo.collection("wursts."+user).findOne({user:user,name:name}, 
   function(err, dbres) {
     if (err) throw err;
     if (dbres && dbres.code) callback(dbres.code);
     else callback();
  });
};


app.get('/', function(req, res) {
  res.send('This is not for you.');
});

app.all('/wurst/:user/:name', function(req, res) {

  var action=req.params.action;
  var user=req.params.user;
  var name=req.params.name;
  var action=req.query.action;
  console.log("Request for "+user+"/"+name+" query "+JSON.stringify(req.query));
  // Validate input
  var OKdigits=/^[A-Za-z0-9_]+$/;
  if (!OKdigits.test(user)) return res.send("Bad chars in user");
  if (!OKdigits.test(name)) return res.send("Bad chars in run name");

// Show crude code editor with ?edit
  if (req.query['edit']=="") {
   return getCode(user,name,function(code) {
     var page='<html><body><h1>Write JavaScript code here</h1>';
     page+='<form method="POST" action="?save">';
     page+='<textarea name="code"  rows="30" cols="85">';
     if (code) page+=code;
     else page+='send("This is <b>cool</b>.");';
     page+='</textarea><br><input type="submit" value="Run It!"></form>';
     page+='</body></html>';
     return res.send(page);
   });
 }

// HTML form callback: save this CGI parameter code
 if (req.query['save']=="") {
  console.log("Code save: query: "+JSON.stringify(req.query)+" body: "+JSON.stringify(req.body));
  var code=req.query['code'] || req.body['code'];
  if (!code) return res.send("Error: no code argument given.");
  console.log("Code saved for "+user+"/"+name+": "+code);
  dbo.collection("wursts."+user).update({user:user, name:name},
	{user:user, name:name, code:code }, {upsert:true});
  return res.redirect(name); // redirect to run page, for testing
 }

// Debug: load saved code
 if (req.query['get']=="") {
  return getCode(user,name,function(code) {
     var out="Code not found.";
     if (code) out=pre(code);
     return res.send(out);
  });
 }

// Utility: list saved code
  if (req.query['list']=="") {
    return dbo.collection("wursts."+user).find({user:user},{name:1}).toArray(
      function(err,array) {
       var list="<ul>";
       for (var idx in array) {
           var n=array[idx].name;
           list+="<li><a href='"+n+"'>"+n+"</a>";
       }
       list+="</ul>";
       return res.send(list);
     });
  }

// Else just run the code
  getCode(user,name,function(code) {
    if (!code) return res.send("Code for "+user+"/"+name+" not found in database");
    console.log("Running "+user+"/"+name+" code: '"+code+"'");

    var consoleText="";
    var sentAlready=false;
    var time_in_seconds=function() { return new Date().getTime()*0.001; };
    var start_time=time_in_seconds();
    try {
    safeEval('function() {'+code+'} ()', {
        mongo: dbo.collection("wursts."+user+".userdata"),
        expressRequest:{query: req.query, body: req.body},
        httpRequest: request,
        escape: escape, 
	timeInSeconds: time_in_seconds,
	setTimeout: function(fn,time) { if (time<=10000) setTimeout(fn,time); },
        JSON: { stringify: JSON.stringify, parse: JSON.parse },
        Math: Math,
        console: { log: function(text) { consoleText+=text+"\n"; } },
        send: function(output) { 
		if (sentAlready) return;
		sentAlready=true;
		var elapsed=time_in_seconds()-start_time;
		res.send(pre(consoleText)+output+"<p align='right'>"+Math.floor(elapsed*1000)+" ms</p>"); 
	}
    });
    } catch (err) {
        res.send(pre(consoleText)+"Exception: "+err+"<br>Source code:"+pre(code));
    }
  });
});

app.listen(8003, function() {
  console.log('wurst app listening');
});

}); // end database access
