var fs = require("fs");

class Led {
	constructor(devpath) {
		this.devpath = devpath;
	}

	setBrightness(brightness) {
		fs.writeFileSync(this.devpath, brightness.toString() + '\n');
	}
}

module.exports = Led;
