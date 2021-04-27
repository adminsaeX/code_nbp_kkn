var mongojs = require("mongojs");
var db_udp = mongojs("solarpump");

/* For UDP connection */
var dgram = require("dgram");
var server = dgram.createSocket("udp4");
var HOST = '0.0.0.0';
var PORT = 41234; // UDP opened port on GCP console.

server.on("listening", function () {
    var address = server.address();
    console.log("Listen udp connection on " + address.address + ":" + address.port);
});

server.on("message", function (message, remote) {
    console.log(remote.address + ":" + remote.port +' - ' + message);
    var strMessage_parse = JSON.parse(message);
    console.log(message);
    //console.log(strMessage_parse.data);

    // Recording string from ESP32 UDP connection into MongoDB

    
    var db_udp_col = db_udp.collection('solar_device_data');

    var dateTime = require('node-datetime');
    var dt = dateTime.create();
    dt.format('d/m/Y H:M:S');
    var ts = new Date(dt.now());

    db_udp_col.insert({
      device_ID: String(strMessage_parse.deviceID),
      device_command:String(strMessage_parse.command),
      device_data:String(strMessage_parse.data),
      TS: Number(ts)

    }, function(err){
      if(err) console.log('record failed');
    });

    // Send response back to client
    var cliresp = new Buffer('200 ok');
    server.send(cliresp, 0, cliresp.length, remote.port, remote.address, function(err, bytes) {
    if (err) throw err;
    console.log('Server response to ' + remote.address +':'+ remote.port + ' | ' + cliresp);
    //client.close();
});

});

server.bind(PORT);

/* For RESTFUL API on mongoDB */
const express = require('express');
const app = express();
var port = 4000;

app.get('/read/:col', function (req, res) {

  var strCol = JSON.stringify(req.params);
  var strCol_parse = JSON.parse(strCol);

  db_udp_read = db_udp.collection(strCol_parse.col);

  db_udp_read.find({}).limit(10).sort({utc:-1}, function(err, docs){
    if(err) return console.log('read failed');
    console.log(docs);
    res.send(docs);
  });
});

app.listen(port, function () {
  var nodeStartTime = new Date();
  console.log('Establish API for mongoDB access : ' + port + ' at ' + nodeStartTime);
});
