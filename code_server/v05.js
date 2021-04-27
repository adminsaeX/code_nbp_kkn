

var Promise = require('promise');
const MongoClient = require("mongodb").MongoClient;
const ObjectId = require("mongodb").ObjectID;
const CONNECTION_URL = "mongodb+srv://adminConnect:adminConnect123@cluster1-mkped.gcp.mongodb.net/test?retryWrites=true&w=majority";
const DATABASE_NAME = "solar_pump";
/* For UDP connection */
var dgram = require("dgram");
var server = dgram.createSocket("udp4");
var HOST = '0.0.0.0';
var PORT = 8011;
var moment = require('moment-timezone');

server.on("listening", function () {
  var address = server.address();
  console.log("Listen udp connection on " + address.address + ":" + address.port);
});

server.on("message", function (message, remote) {
  //console.log(remote.address + ":" + remote.port +' - ' + message);
  var strMessage_parse = JSON.parse(message);


 
  console.log(strMessage_parse);
  

 

 



 










  //console.log(Dev);





  // Send response back to client
  var cliresp = new Buffer('200 ok');
  server.send(cliresp, 0, cliresp.length, remote.port, remote.address, function (err, bytes) {
    if (err) throw err;
    //console.log('Server response to ' + remote.address +':'+ remote.port + ' | ' + cliresp);
    //client.close();
  });

});

server.bind(PORT);

