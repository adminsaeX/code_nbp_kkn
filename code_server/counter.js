


const MongoClient = require("mongodb").MongoClient;
const ObjectId = require("mongodb").ObjectID;
const CONNECTION_URL = "mongodb+srv://adminConnect:adminConnect123@cluster1-mkped.gcp.mongodb.net/test?retryWrites=true&w=majority";
const DATABASE_NAME = "solar_pump";



    MongoClient.connect(CONNECTION_URL, { useNewUrlParser: true }, (error, client) => {
        if(error) {
            throw error;
        }
        var database = client.db(DATABASE_NAME);
        var collection_ID = database.collection("solar_device");
        var collection_Data = database.collection("solar_device_data");
        
          collection_Data.aggregate([
            {
              $match: {
                  'pro_ID' :'NBP1',
                  
              }
          },
          {
              $lookup:
              {
                  from: 'solar_device',
                  localField: 'device_ID',
                  foreignField: 'device_ID',
                  as: 'data'
              }
          }
          ]).toArray(function(err,result){

        if(err||this.status =='DESTOYER'){
          //console.log('error');
          return
        }
        if(result.length === 1){
          console.log(result);
          console.log("connect");
          }
          });
        });
        
   



    
    
    
    
    
    
     
    
     
   
    
   
 

