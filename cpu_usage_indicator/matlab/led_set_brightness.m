function led_set_brightness(devpath, brightness)
  fid = fopen(devpath, "w");
  fprintf(fid, "%d\n", brightness);
  fclose(fid);
