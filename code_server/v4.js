

var mongojs = require("mongojs");
var db_udp = mongojs("solar_pump");

/* For UDP connection */
var dgram = require("dgram");
var server = dgram.createSocket("udp4");
var HOST = '0.0.0.0';
var PORT = 8015; // UDP opened port on GCP console.
var moment = require('moment-timezone');

server.on("listening", function () {
    var address = server.address();
    console.log("Listen udp connection on " + address.address + ":" + address.port);
});

server.on("message", function (message, remote) {
   console.log(remote.address + ":" + remote.port +' - ' + message);
   /*
   var strMessage_parse = JSON.parse(message);
    
    
    var device_id = String(strMessage_parse.dev_ID);
    console.log(device_id);

   
    var time =  moment().tz("Asia/Bangkok").format();
    var timeiso = new Date();
    var d = new Date( new Date().getTime() + 7 * 3600 * 1000);
    var db_udp_col = db_udp.collection('solar_device_data');

    var collection_ID = db_udp.collection('solar_device');

    collection_ID.find({'device_ID':device_id}).toArray(function(err,search){

        if(err||this.status =='DESTOYER'){
          console.log('error');
          return
        }
        if(search.length === 1){
          //console.log(search);
          db_udp_col.insert({
            pro_ID:String(strMessage_parse.pro_ID),
            device_ID: String(strMessage_parse.dev_ID),
            device_command:String(strMessage_parse.com),
            device_data:
            {
              dcV:Number(strMessage_parse.data[0].dcV),
              dcA:Number(strMessage_parse.data[0].dcA),
              dcW:Number(strMessage_parse.data[0].dcW),
              dcWh:Number(strMessage_parse.data[0].dcWh),
              acV:Number(strMessage_parse.data[0].acV),
              acA:Number(strMessage_parse.data[0].acA),
              acW:Number(strMessage_parse.data[0].acW),
              acWh:Number(strMessage_parse.data[0].acWh),
              flow_rate:Number(strMessage_parse.data[0].flow),
              qtotal:Number(strMessage_parse.data[0].qtotal),
              temp:Number(strMessage_parse.data[0].temp),
              tempb:Number(strMessage_parse.data[0].tempB),
              hum:Number(strMessage_parse.data[0].hum),
              fault:Number(strMessage_parse.data[0].fault),
              pump_state:Number(strMessage_parse.data[0].pstate),
              pump_Hz:Number(strMessage_parse.data[0].pHz),
              OutVolt:Number(strMessage_parse.data[0].outV),
              OutAmp:Number(strMessage_parse.data[0].outA),
              OutPower:Number(strMessage_parse.data[0].OutP),
              RPM:Number(strMessage_parse.data[0].rpm),
              solar_Energy:Number(strMessage_parse.data[0].solarE),
              PV_Temp:Number(strMessage_parse.data[0].pvT),
            },
      
            ts: new Date(d) 
      
          }, function(err){
            if(err) console.log('record failed');
          });
        }
    });

    
    
    
    */
    
    
     
    //console.log(Dev);
     
   
    
   
   
    // Send response back to client
    var cliresp = new Buffer('200 ok');
    server.send(cliresp, 0, cliresp.length, remote.port, remote.address, function(err, bytes) {
    if (err) throw err;
    //console.log('Server response to ' + remote.address +':'+ remote.port + ' | ' + cliresp);
    //client.close();
});

});

server.bind(PORT);

