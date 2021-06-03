#include <cassert>
#include <cstring>
#include <fstream>

#include "led.hxx"


using namespace std; 


Led::Led(const char* devpath, unsigned char brightness)
{
	strncpy(this->devpath, devpath, sizeof(this->devpath));
	this->devpath[sizeof(this->devpath) - 1] = '\0';

	set_brightness(brightness);
}


Led::~Led()
{
    set_brightness(0);
}


void Led::set_brightness(unsigned char brightness)
{
	ofstream f(this->devpath);
	assert(f.good());
	f << (unsigned int)brightness << endl;
	f.close();

	this->brightness = brightness;
}

