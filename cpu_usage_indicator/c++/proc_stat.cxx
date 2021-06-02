#include <cassert>
#include <fstream>

#include "proc_stat.hxx"


using namespace std; 


void ProcStat::read()
{
    char discard[4];

	ifstream f("/proc/stat");
	assert(f.good());

	f >> discard >> this->user >> this->system >> this->nice >> this->idle >>
		this->iowait >> this->irq >> this->softirq;

	f.close();
}


void ProcStat::calc()
{
	this->total = this->user;
	this->total += this->nice;
	this->total += this->system;
	this->total += this->idle;
	this->total += this->iowait;
	this->total += this->irq;
	this->total += this->softirq;

	this->used = this->total - this->idle;
}


ProcStat::ProcStat()
{
	read();
	calc();
}


unsigned int  ProcStat::get_cpu_usage()
{
	unsigned int usage;
	ProcStat stat_latest;
	
	stat_latest.read();
	stat_latest.calc();

	usage = (stat_latest.used - this->used) * 100 / (stat_latest.total - this->total);

	this->used = stat_latest.used;
	this->total = stat_latest.total;

	return usage;
}

