var db = require('mongodb').db;
const MongoClient = require('mongodb').MongoClient;
const uri = "mongodb+srv://adminDB:adminDB@1234@cluster0-mkped.gcp.mongodb.net/test?retryWrites=true&w=majority";
const client = new MongoClient(uri, { useNewUrlParser: true });
client.connect(err => {
  const collection = client.db("test").collection("devices");
  // perform actions on the collection object
  console.log('connected');
  client.close();
});