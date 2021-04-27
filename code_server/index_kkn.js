
const MongoClient = require("mongodb").MongoClient;
const ObjectId = require("mongodb").ObjectID;
const CONNECTION_URL = "mongodb+srv://admin:adminsae@cluster1.j2hyh.gcp.mongodb.net/<dbname>?retryWrites=true&w=majority";
const DATABASE_NAME = "solar_pump";
var moment = require('moment-timezone');
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
	var device_id = String(req.body.dev_ID);
    res.send('200');
	console.log(json);
  console.log(device_id)
 if(req.body.pro_ID ==='KKN1'||'KKN2'){ 
  var time = moment().tz("Asia/Bangkok").format();
  var timeiso = new Date();
  var d = new Date(new Date().getTime() + 7 * 3600 * 1000);
  var flowCal = req.body.freq * 0.00195;
 
  var wtotal = flowCal * 0.0167  ;
  //var dcWhC = strMessage_parse.data[0].dcWh * 4;
 // var pumpState = strMessage_parse.data[0].pstate;
  let water;
  let energy;
  let energyN;
  let waterN;
 // var power = (strMessage_parse.data[0].dcV * strMessage_parse.data[0].dcA)/100.0;
  var dcWH = req.body.W * 0.0167;
  var status = req.body.status;
  var rpm = req.body.freq*5.8;
  MongoClient.connect(CONNECTION_URL, { useNewUrlParser: true }, (error, client) => {
    if (error) {
      throw error;
    }
    var database = client.db(DATABASE_NAME);
    var collection_ID = database.collection("solar_device");
    var collection_IDKKN1 = database.collection("solar_device_kkn1")
    var collection_IDKKN2 = database.collection("solar_device_kkn2")
    var collection_Data = database.collection("solar_rawdata");
    var collection_pumpHr = database.collection("solar_device_hr");
    var collection_energy = database.collection("solar_device_dcWh");
    var collection_qtotal = database.collection("solar_device_qtotal");
  //  var collection_log = database.collection('solar_device_log');
    var collection_status = database.collection('solar_device_status');
 
      collection_status.insert(
        {
          _id: String(device_id),
        } ,
        {upsert:true}
        
        );
    collection_ID.find({ 'device_ID': device_id }).toArray( function (err, search) {

      if (err || this.status == 'DESTOYER') {
        //console.log('error');
        return
      }
      if (search.length === 1) {
        //console.log(search);
        /////////////////////////////////////////////////////////////////////////////////////

        if (status === '6') {
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
        collection_status.updateOne(
          { _id: device_id },
          {
            
            $set: { 'pump_status': status }
          },
          { sort: { _id: -1 } },
          { upsert: true }
        );

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

          

          water =  q_total.map((item1) =>
            item1.qtotal
            
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
            collection_IDKKN1.find({ 'device_ID': device_id }).toArray( function (err, search) {

              if (err || this.status == 'DESTOYER') {
                //console.log('error');
                return
              }
              if (search.length === 1) {
            collection_Data.insert({
              pro_ID: String('KKN1'),
              device_ID: String(req.body.dev_ID),
              device_data:
              {
                
                dcV: Number(req.body.V*10),
                dcA: Number(req.body.A),
                dcW: Number(req.body.W),
                dcWh: Number(energyN),
                flow_rate: Number(flowCal),
                qtotal: Number(waterN),
                temp: Number(req.body.temp),
                fault: Number(req.body.alarm),
                RPM:Number(rpm),
                pump_state: Number(req.body.status),
                pump_Hz: Number(req.body.freq),
                pump_run:Number(req.body.statusRun)
              },
              
              ts: new Date(d)
    
            }, function (err) {
              if (err) {
                console.log('record failed');
              }
    
              
            });
              }
          else{
            collection_Data.insert({
              pro_ID: String('KKN2'),
              device_ID: String(req.body.dev_ID),
              device_data:
              {
                
                dcV: Number(req.body.V)*10.0,
                dcA: Number(req.body.A),
                dcW: Number(req.body.W),
                dcWh: Number(energyN),
                flow_rate: Number(flowCal),
                qtotal: Number(waterN),
                temp: Number(req.body.temp),
                fault: Number(req.body.alarm),
                RPM:Number(rpm),
                pump_state: Number(req.body.status),
                pump_Hz: Number(req.body.freq),
                pump_run:Number(req.body.statusRun)
              },
              
              ts: new Date(d)
    
            }, function (err) {
              if (err) {
                console.log('record failed');
              }
    
              
            });
          }
            console.log(energyN);
            console.log( waterN);
          
          });
          });
        });
        //////////////////////////////////////////////////////////////////////////////////
        

        ////////////////////////////////////////////////////////////////////////////////////
        
               
       
        //console.log('waterN before', waterN);

       



      

      }


    });



  });
 }
	
});

app.listen(port, function() {
	console.log('Starting node.js on port ' + port);
});