#ifndef ZMD_LED_H__
#define ZMD_LED_H__

#include <climits>

class Led{
	public:
		Led(const char* devpath, unsigned char brightness);
        ~Led();
		void set_brightness(unsigned char brightness);
	private:
		unsigned char brightness;
		char devpath[PATH_MAX];
};

#endif /* ZMD_LED_H__ */
