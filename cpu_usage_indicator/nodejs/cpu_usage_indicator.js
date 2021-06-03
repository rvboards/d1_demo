var Led = require('./led');
var ProcStat = require('./proc_stat');

var ledr = new Led('/sys/class/leds/sunxi_led0r/brightness');
var ledg = new Led('/sys/class/leds/sunxi_led0g/brightness');
var stat = new ProcStat();

ledr.setBrightness(0);
ledg.setBrightness(0);

var ms = 0;

var timer = setInterval(function () {
	ms += 200;

	if(ms == 800) {
		ledr.setBrightness(0);
		ledg.setBrightness(0);
	} else if(ms == 1000) {
		ms = 0;
		var usage = stat.getCpuUsage();
		process.stdout.write("\rcpu usage = " + usage + " %  ");

		if(usage < 30) {
			ledg.setBrightness(30);
		} else if(usage < 60) {
			ledg.setBrightness(5);
		} else if(usage < 90) {
			ledr.setBrightness(15);
		} else {
			ledr.setBrightness(150);
		}
	}
	

}, 200);

process.on('SIGINT', function () {
	clearInterval(timer);
	ledr.setBrightness(0);
	ledg.setBrightness(0);
	process.stdout.write("\n");
	process.exit();
});
