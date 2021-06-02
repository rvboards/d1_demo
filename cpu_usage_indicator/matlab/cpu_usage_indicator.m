ledr = "/sys/class/leds/sunxi_led0r/brightness";
ledg = "/sys/class/leds/sunxi_led0g/brightness";

obj_clean_up = onCleanup(@()clean_up(ledr, ledg));

led_set_brightness(ledr, 0);
led_set_brightness(ledg, 0);

[stat_used, stat_total] = proc_stat_get_used_and_total();

while 1
  pause(0.8);

  led_set_brightness(ledr, 0);
  led_set_brightness(ledg, 0);
  
  pause(0.2);

  [latest_used, latest_total] = proc_stat_get_used_and_total();

  cpu_usage = (latest_used - stat_used) * 100 / (latest_total - stat_total);
  cpu_usage = round(cpu_usage);
  
  fprintf(stdout, "\rcpu usage = %d %%  ", cpu_usage);
  fflush(stdout);
  
  stat_used = latest_used;
  stat_total = latest_total;
  
  if (cpu_usage < 30)
    led_set_brightness(ledg, 30);
  else 
    if (cpu_usage < 60)
      led_set_brightness(ledg, 5);
    else 
      if cpu_usage < 90
        led_set_brightness(ledr, 15);
      else
        led_set_brightness(ledr, 150);
      end
    end
  end
  
end



