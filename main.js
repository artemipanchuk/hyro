// Require main config
var main = require("./main.json");

// Require main classes
var HTTPServer = require("./server/classes/HTTPServer/HTTPServer"),
	WSServer   = require("./server/classes/WSServer/WSServer");

// Require main modules
var ahrs = require("./server/modules/ahrs"),
	imu  = require("./server/modules/imu");

// Create main objects
var httpServer = new HTTPServer,
	wsServer   = new WSServer;

// Execute main methods
httpServer.run(main.HTTPServerPort);
wsServer.run(main.WSServerPort);

imu.read();

// setInterval(function() {
// 	wsServer.send(JSON.stringify([0, 0, 0, 1]));
// }, 1000)