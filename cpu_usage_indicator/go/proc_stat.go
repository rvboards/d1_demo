package main

import "os"
import "fmt"

type proc_stat_t struct {
	user	uint64
	system	uint64
	nice	uint64
	idle	uint64
	iowait	uint64
	irq		uint64
	softirq	uint64
	total	uint64
	used	uint64
}


func proc_stat_read(stat *proc_stat_t) {
	file, err := os.Open("/proc/stat")
	if err != nil {
		panic(err)
	}

	fmt.Fscanf(file, "cpu %d %d %d %d %d %d %d",
		&stat.user, &stat.system, &stat.nice, &stat.idle,
		&stat.iowait, &stat.irq, &stat.softirq)

	err = file.Close()
	if err != nil {
		panic(err)
	}
}


func proc_stat_calc(stat *proc_stat_t) {
	stat.total = stat.user;
	stat.total += stat.nice;
	stat.total += stat.system;
	stat.total += stat.idle;
	stat.total += stat.iowait;
	stat.total += stat.irq;
	stat.total += stat.softirq;

	stat.used = stat.total - stat.idle;
}


func proc_stat_init(stat *proc_stat_t) {
	proc_stat_read(stat)
	proc_stat_calc(stat)
}


func proc_stat_get_cpu_usage(stat *proc_stat_t) uint8 {
	var stat_latest proc_stat_t

	proc_stat_read(&stat_latest)
	proc_stat_calc(&stat_latest)

	usage := (stat_latest.used - stat.used) * 100 / (stat_latest.total - stat.total)

	stat.used = stat_latest.used;
	stat.total = stat_latest.total;

	return uint8(usage)
}

