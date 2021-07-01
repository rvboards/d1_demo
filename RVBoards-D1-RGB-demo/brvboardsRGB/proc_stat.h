#ifndef ZMD_PROC_STAT_H__
#define ZMD_PROC_STAT_H__

typedef struct proc_stat_s {
	unsigned long long user;
	unsigned long long system;
	unsigned long long nice;
	unsigned long long idle;
	unsigned long long iowait;
	unsigned long long irq;
	unsigned long long softirq;
	unsigned long long total;
	unsigned long long used;
}proc_stat_t;

void proc_stat_init(proc_stat_t* stat);
unsigned int  proc_stat_get_cpu_usage(proc_stat_t* stat);

#endif /* ZMD_PROC_STAT_H__ */
