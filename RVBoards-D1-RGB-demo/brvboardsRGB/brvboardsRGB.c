#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#include "proc_stat.h"
#include "led.h"


typedef struct app_s {
	unsigned int usage;
	proc_stat_t stat;
	led_t ledr;
	led_t ledg;
	led_t ledb;
}app_t;

static app_t app;

volatile int running = 1;

void stop(int signo)
{
    running = 0;
}

void rgb_disp_dome(app_t* app_user, unsigned char* rgb_state, time_t s_rgbrundelay, long us_rgbrundelay)
{
	struct timespec ts, ts_le;
	static unsigned char state_step=7;
	
	if (state_step < 7) {
		switch (state_step) {
		case 0:
			if (rgb_state[0]<255) {
				rgb_state[0]++;
			} else {
				state_step++;
			}

			break;
		case 1:
			if (rgb_state[1] < 255) {
				rgb_state[1]++;
			} else {
				state_step++;
			}
			break;
		case 2:
			if (rgb_state[0] > 0) {
				rgb_state[0]--;
			} else {
				state_step++;
			}
			break;
		case 3:
			if (rgb_state[2] < 255) {
				rgb_state[2]++;
			} else {
				state_step++;
			}
			break;
		case 4:
			if (rgb_state[1] > 0) {
				rgb_state[1]--;
			} else {
				state_step++;
			}
			break;
		case 5:
			if (rgb_state[0] < 255) {
				rgb_state[0]++;
			} else {
				state_step++;
			}
			break;
		case 6:
			if (rgb_state[2] > 0)
			{
				rgb_state[2]--;
			} else {
				state_step=0;
			}
			break;
		default:
			state_step=7;
			break;
		} 
		
	}
	else {
		state_step = 0;
		rgb_state[0] = 0;
		rgb_state[1] = 0;
		rgb_state[2] = 0;
	}

	ts.tv_nsec = us_rgbrundelay ;
	ts.tv_sec = s_rgbrundelay;
	if (nanosleep(&ts, &ts_le) == -1)
	{
		printf("error!\n");
		exit(1);
		//sleep(s_rgbrundelay);
		//usleep(us_rgbrundelay);
	}
}

int main(int argc,char* argv[])
{
	unsigned char rgb[]={0, 0, 0};
	char* str_end="end";char* str_R="r";char* str_G="g";char* str_B="b";char* str_RGB="rgb";
   	signal(SIGINT, stop);
	
	proc_stat_init(&app.stat);
	led_init(&app.ledr, "/sys/class/leds/sunxi_led0r/brightness" , 0);
	led_init(&app.ledg, "/sys/class/leds/sunxi_led0g/brightness" , 0);
	led_init(&app.ledb, "/sys/class/leds/sunxi_led0b/brightness" , 0);

	while(running) {
		if(argc>=2) {
			if(strcmp(argv[1],str_end)==0) { break; }
			if(strcmp(argv[1],str_R)==0) { 
				if(argc>=3){led_set_brightness(&app.ledr, atoi(argv[2])); exit(1); }
				led_set_brightness(&app.ledr, 125); 
				exit(1); 
			}
			if(strcmp(argv[1],str_G)==0) { 
				if(argc>=3){led_set_brightness(&app.ledg, atoi(argv[2])); exit(1); }
				led_set_brightness(&app.ledg, 125); 
				exit(1); 
			}
			if(strcmp(argv[1],str_B)==0) { 
				if(argc>=3){led_set_brightness(&app.ledb, atoi(argv[2])); exit(1); }
				led_set_brightness(&app.ledb, 125); 
				exit(1); 
			}
			if(argc>=3){ 
				if(strcmp(argv[1],str_RGB)==0){ 
					led_set_brightness(&app.ledg, atoi(argv[3]));
					led_set_brightness(&app.ledr, atoi(argv[2]));
					led_set_brightness(&app.ledb, atoi(argv[4]));
					exit(1);
				}
			}
		}
		app.usage = proc_stat_get_cpu_usage(&app.stat);
		printf("cpu usage = %d%% ,RGB = %3d.%3d.%3d\n", app.usage, rgb[0], rgb[1], rgb[2]);

		led_set_brightness(&app.ledg, rgb[1]);
		led_set_brightness(&app.ledr, rgb[0]);
		led_set_brightness(&app.ledb, rgb[2]);

		rgb_disp_dome(&app, rgb, 0, 20000000);
	}
	led_fini(&app.ledr);
	led_fini(&app.ledg);
	led_fini(&app.ledb);
}
