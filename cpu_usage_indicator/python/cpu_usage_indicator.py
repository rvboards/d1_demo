#!/usr/bin/env python3

import time

from led import Led
from proc_stat import ProcStat

class App:
	def __init__(self):
		self.stat = ProcStat()
		self.ledr = Led("/sys/class/leds/sunxi_led0r/brightness" , 0)
		self.ledg = Led("/sys/class/leds/sunxi_led0g/brightness" , 0)

	def run(self):
		time.sleep(0.8)

		self.ledr.set_brightness(0)
		self.ledg.set_brightness(0)

		time.sleep(0.2)

		self.usage = self.stat.get_cpu_usage()
		print("\rcpu usage = %d %%  " % self.usage, end = "")

		
		if self.usage < 30:
			self.ledg.set_brightness(30)
		elif self.usage < 60:
			self.ledg.set_brightness(5)
		elif self.usage < 90:
			self.ledr.set_brightness(15)
		else:
			self.ledr.set_brightness(150)


app = App()

try:
	while 1:
		app.run()
except KeyboardInterrupt:
	print("")
	del app


