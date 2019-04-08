var express = require('express');
var app = express();

var MongoClient = require('mongodb').MongoClient;
var url = "mongodb://localhost:27017/";

MongoClient.connect(url, function(err, db) {
  if (err) throw err;
  var dbo = db.db("mydb");

  app.all('/mongodemo/:key/:val', function(req, res) {

    var key=req.params.key;
    var val=req.params.val;
    var mongo=dbo.collection("mongodemo");

    // Add next request to collection
    var obj={"keyword":key, "value":val};
    mongo.updateOne(obj, {$set:obj}, {upsert:true});

    // Dump all collection data
    return mongo.find({"keyword":key}).toArray(
      function(err,array) {
       var intro="List of data for "+key+":";
       var list="<ul>";
       for (var idx in array) {
           var n=array[idx].value;
           list+="<li>'"+n+"'";
       }
       list+="</ul>";

       return res.send(intro + list);
     });
  });

  app.listen(8102, function() {
    console.log('mongodemo app listening');
  });

}); // end database access
