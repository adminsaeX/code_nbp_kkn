


var app = require('express')();
var bodyParser = require('body-parser');
var port = process.env.PORT || 9999;
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({
	extended: true
}));
app.post('/testdata', function (req, res) {
	var json = req.body;
    res.send('200');
	console.log(json);
	
	
		
});

app.listen(port, function() {
	console.log('Starting node.js on port ' + port);
});