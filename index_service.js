var Promise = require('promise');
const MongoClient = require("mongodb").MongoClient;
const ObjectId = require("mongodb").ObjectID;
const CONNECTION_URL = "mongodb+srv://admin:adminsae@cluster1.j2hyh.gcp.mongodb.net/solar_pump?retryWrites=true&w=majority";
const DATABASE_NAME = "solar_pump";
/* For UDP connection */
var dgram = require("dgram");
var server = dgram.createSocket("udp4");
var HOST = '0.0.0.0';
var PORT = 8010;
var moment = require('moment-timezone');

server.on("listening", function () {
  var address = server.address();
  console.log("Listen udp connection on " + address.address + ":" + address.port);
});

server.on("message", function (message, remote) {
  //console.log(remote.address + ":" + remote.port +' - ' + message);
  var strMessage_parse = JSON.parse(message);


  var device_id = String(strMessage_parse.dev_ID);
  console.log(device_id);


  var time = moment().tz("Asia/Bangkok").format();
  var timeiso = new Date();
  var d = new Date(new Date().getTime() + 7 * 3600 * 1000);
  var flowCal = strMessage_parse.data[0].rpm * 0.00195;
 
  var wtotal = flowCal * 0.0167  ;
  //var dcWhC = strMessage_parse.data[0].dcWh * 4;
 // var pumpState = strMessage_parse.data[0].pstate;
  let water;
  let energy;
  let energyN;
  let waterN;
  var power = (strMessage_parse.data[0].dcV * strMessage_parse.data[0].dcA)/100.0;
  var dcWH = power * 0.0167;
  var status = strMessage_parse.data[0].pstate 
  MongoClient.connect(CONNECTION_URL, { useNewUrlParser: true }, (error, client) => {
    if (error) {
      throw error;
    }
    var database = client.db(DATABASE_NAME);
    var collection_ID = database.collection("solar_device");
    var collection_Data = database.collection("solar_rawdata");
    var collection_pumpHr = database.collection("solar_device_hr");
    var collection_energy = database.collection("solar_device_dcWh");
    var collection_qtotal = database.collection("solar_device_qtotal");
    var collection_log = database.collection('solar_device_log');
    var collection_status = database.collection('solar_device_status');

    collection_ID.find({ 'device_ID': device_id }).toArray( function (err, search) {

      if (err || this.status == 'DESTOYER') {
        //console.log('error');
        return
      }
      if (search.length === 1) {
        //console.log(search);
        /////////////////////////////////////////////////////////////////////////////////////

        if (strMessage_parse.data[0].pstate === '1') {
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
        ///////////////////////////////////////////////////////////////////////////////////////      
        collection_energy.updateOne(
          { 'device_ID': device_id },
          {
            $inc: { 'dcWh': dcWH },
            $set: { 'lastUpdate': new Date(d) }
          },
          { sort: { _id: -1 } },
          { upsert: true }
        );
        ////////////////////////////////////////////////////////////////////////////////////
        collection_qtotal.updateOne(
          { 'device_ID': device_id },
          {
            $inc: { 'qtotal': wtotal },
            $set: { 'lastUpdate': new Date(d) }
          },
          { sort: { _id: -1 } },
          { upsert: true }
        );
        //////////////////////////////////////////////////////////////////////////////////////////////
        if(status === '1'){
        collection_status.updateOne(
          { _id: device_id },
          {

            $set: { 'pump_status':Number(status) }
          },
          { sort: { _id: -1 } },
          { upsert: true }
        );
}
if(status !== '1'){

        collection_status.updateOne(
        {_id: device_id},
        {
                $set:{'pump_status':0}
        },
        {sort:{_id:-1}},
        {upsert:true}
          }
        //////////////////////////////////////////////////////////////////////////////////////////
        collection_qtotal.aggregate([
          {
            $match:
            {
              'device_ID': device_id
            }
          },
          {
            $project:
            {
              'qtotal': '$qtotal'
            }
          }



        ]).toArray().then(q_total =>{
          )
          waterN =  parseFloat(water);

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
              pro_ID: String(strMessage_parse.pro_ID),
              device_ID: String(strMessage_parse.dev_ID),
              device_command: String(strMessage_parse.com),
              device_data:
              {
                dcV: Number(strMessage_parse.data[0].dcV),
                dcA: Number(strMessage_parse.data[0].dcA),
                dcW: Number(strMessage_parse.data[0].OutP),
                dcWh: Number(energyN),
                acV: Number(strMessage_parse.data[0].acV),
                acA: Number(strMessage_parse.data[0].acA),
                acW: Number(strMessage_parse.data[0].acW),
                acWh: Number(strMessage_parse.data[0].acWh),
                flow_rate: Number(flowCal),
                flow_raw:Number(strMessage_parse.data[0].flow),
                qtotal: Number(waterN),
                temp: Number(strMessage_parse.data[0].temp),
                tempb: Number(strMessage_parse.data[0].tempB),
                hum: Number(strMessage_parse.data[0].hum),
                fault: Number(strMessage_parse.data[0].fault),
                pump_state: Number(strMessage_parse.data[0].pstate),
                pump_Hz: Number(strMessage_parse.data[0].pHz),
                OutVolt: Number(strMessage_parse.data[0].outV),
                OutAmp: Number(strMessage_parse.data[0].outA),
                     OutPower: Number(power),
                RPM: Number(strMessage_parse.data[0].rpm),
                solar_Energy: Number(strMessage_parse.data[0].solarE),
                PV_Temp: Number(strMessage_parse.data[0].pvT),
              },

              ts: new Date(d)
    
            }, function (err) {
              if (err) {
                console.log('record failed');
              }
    

            });

            console.log(energyN);
            console.log( waterN);
                client.close();
          });
          console.log(energyN);
            console.log( waterN);
                client.close();
          });

        });
        //////////////////////////////////////////////////////////////////////////////////


        ////////////////////////////////////////////////////////////////////////////////////



        //console.log('waterN before', waterN);







      }


    });
// client.close();
  });










  //console.log(Dev);

  // client.close();



  // Send response back to client
  var cliresp = new Buffer('200 ok');
  server.send(cliresp, 0, cliresp.length, remote.port, remote.address, function (err, bytes) {
    if (err) throw err;
    //console.log('Server response to ' + remote.address +':'+ remote.port + ' | ' + cliresp);
   // client.close();
  });

});

server.bind(PORT);
