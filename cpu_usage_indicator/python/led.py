class Led:
	def __init__(self, devpath, brightness):
		self.devpath = devpath
		self.brightness = brightness

	def __del__(self):
		self.set_brightness(0)

	def set_brightness(self, brightness):
		f = open(self.devpath, "w")
		f.write("%d\n" % brightness)
		f.close();

		self.brightness = brightness
