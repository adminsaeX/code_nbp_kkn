


const MongoClient = require("mongodb").MongoClient;
const ObjectId = require("mongodb").ObjectID;
const CONNECTION_URL = "mongodb+srv://adminConnect:adminConnect123@cluster1-mkped.gcp.mongodb.net/test?retryWrites=true&w=majority";
const DATABASE_NAME = "solar_pump";
var water;
var w;

    MongoClient.connect(CONNECTION_URL, { useNewUrlParser: true }, (error, client) => {
        if(error) {
            throw error;
        }
        var database = client.db(DATABASE_NAME);
        var collection_ID = database.collection("solar_device");
        var collection_Data = database.collection("solar_device_qtotal");
        var collection_Data2 = database.collection("solar_device_dcWh");
          collection_ID.aggregate(
            
              [{
                $match: {
                pro_ID:'NBP1',
                district:'เมืองหนองบัวลำภู'
              }},
               {$lookup: {
                from: 'solar_device_qtotal',
                localField: 'device_ID',
                foreignField: 'device_ID',
                as: 'water'
              }}, {$unwind: {
                path: '$water',
              
                 preserveNullAndEmptyArrays: true
              }}, {$project: {
                water:'$water.qtotal',
                date:'$water.lastUpdate'

              }},{$group:
                {
                  _id: { month: { $month: "$date"}, year: { $year: "$date" } },
                  totalWater: { $sum:'$water'}



                }


              }
            
        
          ]).toArray().then(water => {

              console.log(water);
              
              

            
            });
            
        
         
          //console.log(data);
          
        
         

          
        });
        
   



    
    
    
    
    
    
     
    
     
   
    
   
 

