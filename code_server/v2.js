

/* For UDP connection */
var dgram = require("dgram");
var server = dgram.createSocket("udp4");
var HOST = '0.0.0.0';
var PORT = 9000 ; // UDP opened port on GCP console.
var moment = require('moment-timezone');
server.on("listening", function () {
    var address = server.address();
    console.log("Listen udp connection on " + address.address + ":" + address.port);
});

server.on("message", function (message, remote) {
   console.log(remote.address + ":" + remote.port +' - ' + message);
    
   
   
    // Send response back to client
    var cliresp = new Buffer('200 ok');
    server.send(cliresp, 0, cliresp.length, remote.port, remote.address, function(err, bytes) {
    if (err) throw err;
    console.log('Server response to ' + remote.address +':'+ remote.port + ' | ' + cliresp);
    //client.close();
});

});

server.bind(PORT);
