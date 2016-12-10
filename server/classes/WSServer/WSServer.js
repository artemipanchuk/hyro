var WebSocketServer = require("ws");

var WSServer = (function() {
	var uid     = 0,
		clients = {};

	function WSServer() {
	}

	WSServer.prototype.run = function(port, callback) {
		var server = new WebSocketServer.Server({
			port: port
		});

		server.on('connection', function(ws) {
			var id          = ++uid;
				clients[id] = ws;

			ws.on('message', function(message) {
				callback(message);
			});

			ws.on('close', function() {
				delete clients[id];
			});
		});
	};

	WSServer.prototype.send = function(data) {
		for (var id in clients) if (clients.hasOwnProperty(id)) {
			var ws = clients[id];

			ws.send(data);
		}
	};

	return WSServer;
})();

module.exports = WSServer;