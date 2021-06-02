use std::fs;

pub struct ProcStat {
	user:		u64,
	system:		u64,
	nice:		u64,
	idle:		u64,
	iowait:		u64,
	irq:		u64,
	softirq:	u64,
	total:		u64,
	used:		u64,
}

impl ProcStat {
	pub fn new() -> ProcStat {
		let mut stat = ProcStat {
			user: 0,
			system: 0,
			nice: 0,
			idle: 0,
			iowait: 0,
			irq: 0,
			softirq: 0,
			total: 0,
			used: 0,
		};

		stat.read();
		stat.calc();

		stat
	}

	fn read(&mut self) {
		let line = fs::read_to_string("/proc/stat").unwrap();
		let mut iter = line.split_whitespace();
		iter.next();
		self.user = iter.next().unwrap().parse().unwrap();
		self.system = iter.next().unwrap().parse().unwrap();
		self.nice = iter.next().unwrap().parse().unwrap();
		self.idle = iter.next().unwrap().parse().unwrap();
		self.iowait = iter.next().unwrap().parse().unwrap();
		self.irq = iter.next().unwrap().parse().unwrap();
		self.softirq = iter.next().unwrap().parse().unwrap();
	}

	fn calc(&mut self) {
		self.total = self.user;
		self.total += self.nice;
		self.total += self.system;
		self.total += self.idle;
		self.total += self.iowait;
		self.total += self.irq;
		self.total += self.softirq;

		self.used = self.total - self.idle;
	}

	pub fn get_cpu_usage(&mut self) -> u8 {
		let stat_latest = ProcStat::new();
		
		let usage = (stat_latest.used - self.used) * 100 / (stat_latest.total - self.total);

		self.used = stat_latest.used;
		self.total = stat_latest.total;

		 usage as u8
	}
}