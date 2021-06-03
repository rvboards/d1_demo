var fs = require("fs");

class ProcStat {
	constructor() {
		this._read();
		this._calc();
	}

	_read() {
		var line = fs.readFileSync("/proc/stat", { encoding: "utf8" });
		var values = line.split(" ");
		var i = 1;
		while (values[i] == ' ' || values[i] == '') {
			i++;
		}
		this.user = parseInt(values[i + 0]);
		this.system = parseInt(values[i + 1]);
		this.nice = parseInt(values[i + 2]);
		this.idle = parseInt(values[i + 3]);
		this.iowait = parseInt(values[i + 4]);
		this.irq = parseInt(values[i + 5]);
		this.softirq = parseInt(values[i + 6]);
	}

	_calc() {
		this.total = this.user;
		this.total += this.nice;
		this.total += this.system;
		this.total += this.idle;
		this.total += this.iowait;
		this.total += this.irq;
		this.total += this.softirq;

		this.used = this.total - this.idle;
	}

	getCpuUsage() {
		var stat_latest = new ProcStat();
		
		var usage = (stat_latest.used - this.used) * 100 / (stat_latest.total - this.total);

		this.used = stat_latest.used;
		this.total = stat_latest.total;

		return Math.round(usage);
	}
}

module.exports = ProcStat;
