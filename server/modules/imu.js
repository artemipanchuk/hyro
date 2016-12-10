var fs = require("fs");

var imu = (function() {
	var imu = {};

	imu.read = function() {
		var data = fs.readFileSync('/dev/gy80', 'utf8');

		console.log(data)
	};

	return imu;
})();

module.exports = imu;