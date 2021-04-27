
const MongoClient = require("mongodb").MongoClient;
const ObjectId = require("mongodb").ObjectID;
const CONNECTION_URL = "mongodb+srv://adminConnect:adminConnect123@cluster1-mkped.gcp.mongodb.net/test?retryWrites=true&w=majority";
const DATABASE_NAME = "solar_pump";

var http = require("http");
var options = {
  hostname: '178.128.219.211',
  port: 1880,
  path: '/alarm',
  method: 'POST',
  headers: {
      'Content-Type': 'application/json',
  }
};



var app = require('express')();
var bodyParser = require('body-parser');
var port = process.env.PORT || 9010;
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({
	extended: true
}));
app.post('/data', function (req, res) {
	var json = req.body;
    res.send('200');
    //console.log(json);
   
    
});
app.listen(port, function() {
	console.log('Starting node.js on port ' + port);
});
