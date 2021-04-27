


const MongoClient = require("mongodb").MongoClient;
const ObjectId = require("mongodb").ObjectID;
const CONNECTION_URL = "mongodb+srv://adminConnect:adminConnect123@cluster1-mkped.gcp.mongodb.net/test?retryWrites=true&w=majority";
const DATABASE_NAME = "solar_pump";

var w;
let Water;
    MongoClient.connect(CONNECTION_URL, { useNewUrlParser: true }, (error, client) => {
        if(error) {
            throw error;
        }
        var database = client.db(DATABASE_NAME);
        var collection_ID = database.collection("solar_device");
        var collection_water = database.collection("solar_water_nbp");
        
      
        
         collection_ID.aggregate(
            
            [{$match: {
                pro_ID:'NBP1',
                district:'เมืองหนองบัวลำภู',
                sub_district:'กุดจิก'
              }}, {$lookup: {
                from: 'solar_device_qtotal',
                localField: 'device_ID',
                foreignField: 'device_ID',
                as: 'data'
              }}, {$unwind: {
                path: '$data',
              
                  preserveNullAndEmptyArrays: true
              }}, {$project: {
                water :"$data.qtotal"
              }}, {$group: {
                _id: {},
                water :{$sum:'$water'}
              }}
        
          ]).toArray().then(water => {

            console.log(water);
           
      /*
                collection_water.updateOne(
                {
                 'district':'เมืองหนองบัวลำภู',
                 'sub_district':'กุดจิก'
                },
                 {
                     $set:{'data':Object(water) }

                 },

                 { upsert: true }

                );
              
        */
         
          //
          
        });
         

          
        });
        
   



    
    
    
    
    
    
     
    
     
   
    
   
 

