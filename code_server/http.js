var PORT = 8888
const server = mainApp.listen(PORT)
const express = require('express')
const app = express()
const devicesApp = express.Router()
//app.setup(server);

server.on('listening', () => {
  logger.info('application started on port:' + PORT)
})

devicesApp.post('/devices', function (req, res) {
  var json = req.body;
  res.send('200');
  console.log(json);
})
module.exports = devicesApp