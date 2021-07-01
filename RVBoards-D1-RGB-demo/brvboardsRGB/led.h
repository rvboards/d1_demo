#ifndef ZMD_LED_H__
#define ZMD_LED_H__

#include <limits.h>

typedef struct led_s {
	unsigned char brightness;
	char devpath[PATH_MAX];
}led_t;


void led_init(led_t* led, const char* devpath, unsigned char brightness);
void led_fini(led_t* led);
void led_set_brightness(led_t* led, unsigned char brightness);


#endif /* ZMD_LED_H__ */
