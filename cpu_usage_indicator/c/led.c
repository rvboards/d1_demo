#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "led.h"

void led_init(led_t* led, const char* devpath, unsigned char brightness)
{
	strncpy(led->devpath, devpath, sizeof(led->devpath));
	led->devpath[sizeof(led->devpath) - 1] = '\0';

	led_set_brightness(led, brightness);
}


void led_fini(led_t* led)
{
    led_set_brightness(led, 0);
}


void led_set_brightness(led_t* led, unsigned char brightness)
{
	FILE* f = fopen(led->devpath, "w");
	assert(f != NULL);
	fprintf(f, "%d\n", brightness);
	fclose(f);

	led->brightness = brightness;
}

