
const MongoClient = require("mongodb").MongoClient;
const ObjectId = require("mongodb").ObjectID;
const CONNECTION_URL = "mongodb+srv://adminConnect:adminConnect123@cluster1-mkped.gcp.mongodb.net/test?retryWrites=true&w=majority";
const DATABASE_NAME = "db_test";

var app = require('express')();
var bodyParser = require('body-parser');
var port = process.env.PORT || 8111;
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({
	extended: true
}));
app.get('/senddata/:dev_ID/:lat/:lon/:d0/:d1/:d2/:d3/:d4/:d5', function (req, res) {
	const { params} = req;
    res.send('200');
	console.log(params);
	
	
	var device_id = String(req.params.dev_ID);
	var d = new Date( new Date().getTime() + 7 * 3600 * 1000);

    
    MongoClient.connect(CONNECTION_URL, { useNewUrlParser: true }, (error, client) => {
        if(error) {
            throw error;
        }
    var database = client.db(DATABASE_NAME);
    var collection_ID = database.collection('device_test');
    var collection_data = database.collection('data_test');
    console.log(req.body.dev_ID);
	collection_ID.find({'device_ID':device_id}).toArray(function(err,search){

        if(err||this.status =='DESTOYER'){
          console.log('error');
          return
        }
        if(search.length === 1){
			collection_data.insert({
				//pro_ID:String(req.body.pro_ID),
				device_ID: String(req.params.dev_ID),
				device_data:
				{
					data0:Number(req.params.d0),
					data1:Number(req.params.d1),
					data2:Number(req.params.d2),
					data3:Number(req.params.d3),
					data4:Number(req.params.d4),
					data5:Number(req.params.d5)
					
				
				  },
			    gps_data:
				{
					lat:Number(req.params.lat),
					lon:Number(req.params.lon)
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