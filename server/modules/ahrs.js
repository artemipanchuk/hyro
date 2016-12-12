var AHRS = require("ahrs");

var ahrs = (function() {
	var madgwick = new AHRS({
		sampleInterval: 50,
		beta: 0.1,
		algorithm: 'Madgwick'
	}), ahrs = {};

	ahrs.process = function(array) {
		madgwick.update.apply(madgwick, array);

		return madgwick.getQuaternion();
	};

	return ahrs;
})();

module.exports = ahrs;