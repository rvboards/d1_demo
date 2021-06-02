function clean_up(ledr, ledg)
  led_set_brightness(ledr, 0);
  led_set_brightness(ledg, 0);
  fprintf(stdout, "\n");
end
