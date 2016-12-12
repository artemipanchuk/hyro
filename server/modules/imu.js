var fs = require("fs");

var imu = (function() {
	var imu = {},
		deg2rad = 3.1415926/180.0/30,
		lastCall;

	imu.read = function() {
		var data = fs.readFileSync('/dev/gy80', 'utf8').trim(),
			array = data.split(/ +/).map(function(value) {
				return parseInt(value);
			});

		array[0] = Math.abs(array[0]) < 20 ? 0 : array[0];
		array[1] = Math.abs(array[1]) < 20 ? 0 : array[1];
		array[2] = Math.abs(array[2]) < 20 ? 0 : array[2];

		array[0] *= deg2rad;
		array[1] *= deg2rad;
		array[2] *= deg2rad;

		var thisCall = +(new Date)/1000

		if (lastCall) {
			var delta = thisCall - lastCall;

			array.push(delta);
		}

		lastCall = thisCall;

		return array;
	};

	return imu;
})();

module.exports = imu;