#ifndef ZMD_PROC_STAT_H__
#define ZMD_PROC_STAT_H__

class ProcStat{
	public:
		ProcStat();
		unsigned int  get_cpu_usage();
	protected:
		void read();
		void calc();
	private:
		unsigned long long user;
		unsigned long long system;
		unsigned long long nice;
		unsigned long long idle;
		unsigned long long iowait;
		unsigned long long irq;
		unsigned long long softirq;
		unsigned long long total;
		unsigned long long used;
};


#endif /* ZMD_PROC_STAT_H__ */
