#include <unistd.h>

#include <csignal>
#include <iostream>

#include "proc_stat.hxx"
#include "led.hxx"


using namespace std;


class App {
	public:
		App();
		void run();
	private:
		unsigned int usage;
		ProcStat stat;
		Led ledr;
		Led ledg;
};


App::App(void)
	:stat(),
	 ledr("/sys/class/leds/sunxi_led0r/brightness" , 0),
	 ledg("/sys/class/leds/sunxi_led0g/brightness" , 0)
{

}

void App::run(void)
{
	usleep(800000);

	ledr.set_brightness(0);
	ledg.set_brightness(0);

	usleep(200000);

	this->usage = stat.get_cpu_usage();
	cout << "\rcpu usage = " << this->usage << " %  " << flush;

	if(this->usage < 30) {
		ledg.set_brightness(30);
	} else if(this->usage < 60) {
		ledg.set_brightness(5);
	} else if(this->usage < 90) {
		ledr.set_brightness(15);
	} else {
		ledr.set_brightness(150);
	}
}


App app;

volatile int running = 1;

void stop(int signo)
{
	running = 0;
}

int main()
{
	signal(SIGINT, stop);

	while(running) {
		app.run();
	}
	cout << endl;
}
