from functools import reduce

stat_list = list(["user", "system", "nice", "idle", "iowait", "irq", "softirq"])

class ProcStat:
	def __init__(self):
		self.read()
		self.calc()

	def get_cpu_usage(self):
		stat_latest = ProcStat()
		
		usage = (stat_latest.used - self.used) * 100 / (stat_latest.total - self.total)

		self.used = stat_latest.used
		self.total = stat_latest.total

		return usage

	def read(self):
		with open("/proc/stat", "r") as f:
			[setattr(self, k, int(v))
				for k, v in zip(stat_list, f.readline().split()[1:])]

	def calc(self):
		self.total = reduce(lambda x, y: x + y, [getattr(self, k) for k in stat_list])
		self.used = self.total - self.idle;
