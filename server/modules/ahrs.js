var AHRS = require("ahrs");

var ahrs = (function() {
	var madgwick = new AHRS({
		sampleInterval: 1,
		algorithm: 'Madgwick',
		/*
		* The filter noise value, smaller values have
		* smoother estimates, but have higher latency.
		* This only works for the `Madgwick` filter.
		*/
		beta: 0.4,
		/*
		* The filter noise values for the `Mahony` filter.
		*/
		kp: 0.5,
		ki: 0
	}), ahrs = {};

	ahrs.process = function(gx, gy, xz, ax, ay, az, mz, my, mz) {
		madgwick.update(gx, gy, xz, ax, ay, az, mz, my, mz);

		return madgwick.getQuaternion();
	};

	return ahrs;
})();

module.exports = ahrs;