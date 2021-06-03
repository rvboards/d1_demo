cpu_usage_indicator: cpu_usage_indicator.cxx led.cxx proc_stat.cxx Makefile
	g++ -Wall -g -o cpu_usage_indicator cpu_usage_indicator.cxx led.cxx proc_stat.cxx

clean:
	rm -rf cpu_usage_indicator

run: cpu_usage_indicator
	sudo ./cpu_usage_indicator

