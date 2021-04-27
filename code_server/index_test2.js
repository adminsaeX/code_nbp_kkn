
const MongoClient = require("mongodb").MongoClient;
const ObjectId = require("mongodb").ObjectID;
const CONNECTION_URL = "mongodb+srv://adminConnect:adminConnect123@cluster1-mkped.gcp.mongodb.net/test?retryWrites=true&w=majority";
const DATABASE_NAME = "solar_pump";

var app = require('express')();
var bodyParser = require('body-parser');
var port = process.env.PORT || 8111;
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({
	extended: true
}));
app.post('/data', function (req, res) {
	var json = req.body;
    res.send('200');
	console.log(json);
	
	
	var device_id = String(req.body.dev_ID);
	var d = new Date( new Date().getTime() + 7 * 3600 * 1000);
    
    
    MongoClient.connect(CONNECTION_URL, { useNewUrlParser: true }, (error, client) => {
        if(error) {
            throw error;
        }
    var database = client.db(DATABASE_NAME);
    var collection_ID = database.collection('solar_device');
    var collection_data = database.collection('solar_device_data');
    console.log(req.body.dev_ID);
	collection_ID.find({'device_ID':device_id}).toArray(function(err,search){

        if(err||this.status =='DESTOYER'){
          console.log('error');
          return
        }
        if(search.length === 1){
			collection_data.insert({
				pro_ID:String(req.body.pro_ID),
				device_ID: String(req.body.dev_ID),
				device_data:
				{
					dcV:Number(req.body.dcV),
					dcA:Number(req.body.dcA),
					dcW:Number(req.body.dcW),
					dcWh:Number(req.body.acWh),
					acV:Number(req.body.acV),
					acA:Number(req.body.acA),
					acW:Number(req.body.acW),
					acWh:Number(req.body.acWh),
					flow_rate:Number(req.body.flow),
					qtotal:Number(req.body.qtotal),
					temp:Number(req.body.temp),
					hum:Number(req.body.hum)
				
				  },
			
				  ts: new Date(d) 
			
				}, function(err){
				  if(err) console.log('record failed');
				});
			  }

			});
    });
});

app.listen(port, function() {
	console.log('Starting node.js on port ' + port);
});