#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <signal.h>

#include "proc_stat.h"
#include "led.h"


typedef struct app_s {
	unsigned int usage;
	proc_stat_t stat;
	led_t ledr;
	led_t ledg;
}app_t;

static app_t app;

volatile int running = 1;

void stop(int signo)
{
	running = 0;
}

int main()
{
	signal(SIGINT, stop);

	proc_stat_init(&app.stat);
	led_init(&app.ledr, "/sys/class/leds/sunxi_led0r/brightness" , 0);
	led_init(&app.ledg, "/sys/class/leds/sunxi_led0g/brightness" , 0);

	while(running) {
		usleep(800000);

		led_set_brightness(&app.ledr, 0);
		led_set_brightness(&app.ledg, 0);

		usleep(200000);

		app.usage = proc_stat_get_cpu_usage(&app.stat);
		printf("\rcpu usage = %d %%  ", app.usage);
		fflush(stdout);

		if(app.usage < 30) {
			led_set_brightness(&app.ledg, 30);
		} else if(app.usage < 60) {
			led_set_brightness(&app.ledg, 5);
		} else if(app.usage < 90) {
			led_set_brightness(&app.ledr, 15);
		} else {
			led_set_brightness(&app.ledr, 150);
		}
	}

	printf("\n");
	led_fini(&app.ledr);
	led_fini(&app.ledg);
}
