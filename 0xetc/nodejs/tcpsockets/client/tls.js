let tls = require('tls');
let fs = require('fs');
let config = require('./config');

let options = {
  
//  key: fs.readFileSync('client-key.pem'),
//  cert: fs.readFileSync('client-cert.pem'),


  ca: [ fs.readFileSync(config.ROOT_CA_PATH) ]
};

const socket = tls.connect(9001, 'server', options);

socket.on("data", function (data) {
  console.log(data.toString());
});

socket.on("close", function() {
  socket.close();
});

socket.write("oh hello from client tls")