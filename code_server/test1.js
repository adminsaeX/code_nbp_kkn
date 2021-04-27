
     
const MongoClient = require('mongodb').MongoClient;
const uri = "mongodb+srv://<username>:<password>@cluster1-mkped.gcp.mongodb.net/test?retryWrites=true&w=majority";
const client = new MongoClient(uri, { useNewUrlParser: true });
client.connect(err => {
  const collection = client.db("db_test").collection("devices");
  // perform actions on the collection object
  collection.insert({dev_ID:'1234'});
  client.close();
});
     
            
        
 