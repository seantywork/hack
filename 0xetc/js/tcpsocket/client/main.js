var net = require('net');

var client = new net.Socket();
client.connect(9000, "server");

client.on('data', function(data) {
	console.log('client receive: ' + data);
});

client.on('close', function() {
	console.log('connection closed');
});


client.write("oh hello from client")