define([], function() {
	this.Socket = (function() {
		var socket;

		function Socket (url, callback) {
			var socket = new WebSocket(url);

			socket.onopen = function() {
				console.log('INFO: Socket opened ok')
			};

			socket.onclose = function(event) {
				if (event.wasClean) {
					console.log('INFO: Socket closed ok');
				} else {
					console.log('ERROR: Socket closed with error');
				}
			};

			socket.onmessage = function(event) {
				console.log('INFO: Socket message received ok')
				callback(event.data);
			};

			socket.onerror = function(error) {
				console.log('ERROR: Socket encountered error ' + error.message);
			};
		}

		return Socket;
	})();
});