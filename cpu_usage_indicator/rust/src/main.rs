extern crate ctrlc;
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::Arc;
use std::{thread, time, io};
use std::io::Write;


fn main() {
	let running = Arc::new(AtomicBool::new(true));
	let r = running.clone();

	ctrlc::set_handler(move || {
		r.store(false, Ordering::SeqCst);
	}).expect("Error setting Ctrl-C handler");

	let mut stat = cpu_usage_indicator::ProcStat::new();
	let mut ledr = cpu_usage_indicator::Led::new(String::from("/sys/class/leds/sunxi_led0r/brightness"));
	let mut ledg = cpu_usage_indicator::Led::new(String::from("/sys/class/leds/sunxi_led0g/brightness"));


	while running.load(Ordering::SeqCst) {
		thread::sleep(time::Duration::from_millis(800));

		ledr.set_brightness(0);
		ledg.set_brightness(0);

		thread::sleep(time::Duration::from_millis(200));

		let usage = stat.get_cpu_usage();
		print!("\rcpu usage = {} %  ", usage);
		io::stdout().flush().unwrap();

		if usage < 30 {
			ledg.set_brightness(30);
		} else if usage < 60 {
			ledg.set_brightness(5);
		} else if usage < 90 {
			ledr.set_brightness(15);
		} else {
			ledr.set_brightness(150);
		}
	}

	println!();
}
