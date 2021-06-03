cpu_usage_indicator: cpu_usage_indicator.c led.c proc_stat.c Makefile
	gcc -Wall -g -o cpu_usage_indicator cpu_usage_indicator.c led.c proc_stat.c

clean:
	rm -rf cpu_usage_indicator

run: cpu_usage_indicator
	sudo ./cpu_usage_indicator

