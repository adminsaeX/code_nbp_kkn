const MongoClient = require("mongodb").MongoClient;
const ObjectId = require("mongodb").ObjectID;
const CONNECTION_URL = "mongodb+srv://adminConnect:adminConnect123@cluster1-mkped.gcp.mongodb.net/test?retryWrites=true&w=majority";
const DATABASE_NAME = "solar_pump";

MongoClient.connect(CONNECTION_URL, { useNewUrlParser: true }, (error, client) => {
    if(error) {
        console.log('failed')
        throw error;
    }
        var database = client.db(DATABASE_NAME);
        
        var collection_pumpHr = database.collection("solar_device_hr");

    
       
            collection_pumpHr.insert({

                device_ID:'12345'
            
              
            },function(error){
                if(error) {console.log('record failed');
            }else
            {
                console.log('update');
                MongoClient.close();
            }
              
        });
            
          
            
        
        
            
        },function(error){
            if(error) {console.log('record failed');
        }
          
        });