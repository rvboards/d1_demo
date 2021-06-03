use std::fs;

pub struct Led {
	devpath: String,
}

impl Led {
	pub fn new(devpath: String) -> Led {
		let mut l = Led {
			devpath: devpath,
		};

		l.set_brightness(0);

		l
	}

	pub fn set_brightness(&mut self, brightness: u8) {
		fs::write(&self.devpath, format!("{}\n", brightness).as_bytes()).unwrap();
	}
}

impl Drop for Led {
	fn drop(&mut self) {
		self.set_brightness(0);
	}
}