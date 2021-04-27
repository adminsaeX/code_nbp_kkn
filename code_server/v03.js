
const MongoClient = require("mongodb").MongoClient;
const ObjectId = require("mongodb").ObjectID;
const CONNECTION_URL = "mongodb+srv://adminConnect:adminConnect123@cluster1-mkped.gcp.mongodb.net/test?retryWrites=true&w=majority";
const DATABASE_NAME = "solar_pump";


/* For UDP connection */
var dgram = require("dgram");
var server = dgram.createSocket("udp4");
var HOST = '0.0.0.0';
var PORT = 8011; // UDP opened port on GCP console.
var moment = require('moment-timezone');

server.on("listening", function () {
    var address = server.address();
    console.log("Listen udp connection on " + address.address + ":" + address.port);
});

server.on("message", function (message, remote) {
   //console.log(remote.address + ":" + remote.port +' - ' + message);
    var buf = Buffer.alloc(1024);
    buf = message
    var strMessage_parse = JSON.parse(buf);
    
    
    var device_id = String(strMessage_parse.dev_ID);
    console.log(device_id);

   
    var time =  moment().tz("Asia/Bangkok").format();
    var timeiso = new Date();
    var d = new Date( new Date().getTime() + 7 * 3600 * 1000);
    
    MongoClient.connect(CONNECTION_URL, { useNewUrlParser: true }, (error, client) => {
      if(error) {
          throw error;
      }
      var database = client.db(DATABASE_NAME);
      var collection_ID = database.collection("solar_device");
      var collection_Data = database.collection("solar_device_data");
      var collection_pumpHr = database.collection("solar_device_hr");
      var collection_energy = database.collection("solar_device_dcWh");
      var dcWh = strMessage_parse.data[0].OutP*0.0167;
      var energy;
      var energyN;
      collection_ID.find({'device_ID':device_id}).toArray(function(err,search){

      if(err||this.status =='DESTOYER'){
        //console.log('error');
        return
      }
      if(search.length === 1){
        //console.log(search);
        
        if (strMessage_parse.data[0].OutPower != '0') {
          collection_pumpHr.updateOne(
            { 'device_ID': device_id },
            {
              $inc: { 'pump_hr': 1 / 60 },
              $set: { 'lastUpdate': new Date(d) }
            },
            { sort: { _id: -1 } },
            { upsert: true }

          );
        }

        collection_energy.updateOne(
          { 'device_ID': device_id },
          {
            $inc: { 'dcWh': dcWH },
            $set: { 'lastUpdate': new Date(d) }
          },
          { sort: { _id: -1 } },
          { upsert: true }
        );

        collection_energy.aggregate([
            {
              $match:
              {
                'device_ID': device_id
              }
            },
            {
              $project:
              {
                'dcWh': '$dcWh'
              }
            }
  
  
  
          ]).toArray().then(e_nergy=> {
  
            
            energy =  e_nergy.map((item) =>
              item.dcWh
            )
  
            energyN =  parseFloat(energy);
           collection_Data.insert({
          pro_ID:String(strMessage_parse.pro_ID),
          device_ID: String(strMessage_parse.dev_ID),
          device_data:
          {
            dcV:Number(strMessage_parse.data[0].dcV),
            dcA:Number(strMessage_parse.data[0].dcA),
            dcW:Number(strMessage_parse.data[0].dcW),
            dcWh:Number(energyN),
            fault:Number(strMessage_parse.data[0].fault),
            OutPower:Number(strMessage_parse.data[0].OutP)

            
          },
    
          ts: new Date(d) 
    
        }, function(err){
          if(err) console.log('record failed');
        });

       

        });
          
        }

     
  });
     
  });
    
    
    
    
    
     
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

