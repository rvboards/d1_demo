#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "proc_stat.h"

static void proc_stat_read(proc_stat_t* stat)
{
	FILE* f = fopen("/proc/stat", "r");
	assert(f != NULL);

	fscanf(f, "cpu %llu %llu %llu %llu %llu %llu %llu",
		&stat->user, &stat->system, &stat->nice, &stat->idle,
		&stat->iowait, &stat->irq, &stat->softirq);

	fclose(f);
}


static void proc_stat_calc(proc_stat_t* stat)
{
	stat->total = stat->user;
	stat->total += stat->nice;
	stat->total += stat->system;
	stat->total += stat->idle;
	stat->total += stat->iowait;
	stat->total += stat->irq;
	stat->total += stat->softirq;

	stat->used = stat->total - stat->idle;
}


void proc_stat_init(proc_stat_t* stat)
{
	proc_stat_read(stat);
	proc_stat_calc(stat);
}


unsigned int  proc_stat_get_cpu_usage(proc_stat_t* stat)
{
	unsigned int usage;
	proc_stat_t stat_latest;
	
	proc_stat_read(&stat_latest);
	proc_stat_calc(&stat_latest);

	usage = (stat_latest.used - stat->used) * 100 / (stat_latest.total - stat->total);

	stat->used = stat_latest.used;
	stat->total = stat_latest.total;

	return usage;
}

