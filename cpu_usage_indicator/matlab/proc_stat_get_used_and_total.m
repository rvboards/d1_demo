function [stat_used, stat_total] = proc_stat_get_used_and_total()
  fid = fopen("/proc/stat");
  [values, count, errmsg] = fscanf(fid, "cpu %d %d %d %d %d %d %d");
  fclose(fid);
  
  stat_idle = values(4);
  stat_total = sum(values);
  stat_used = stat_total - stat_idle;
