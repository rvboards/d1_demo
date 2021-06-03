package main

import "fmt"
import "time"
import "os"
import "os/signal"


type app_t struct {
	running chan os.Signal
	usage uint8
	ledr led_t
	ledg led_t
	stat proc_stat_t
}


func main() {
	var app app_t

	app.running = make(chan os.Signal, 1)
	signal.Notify(app.running, os.Interrupt, os.Kill)

	proc_stat_init(&app.stat)
	led_init(&app.ledr, "/sys/class/leds/sunxi_led0r/brightness")
	led_init(&app.ledg, "/sys/class/leds/sunxi_led0g/brightness")

main_loop:
	for true {
		time.Sleep(time.Millisecond * 800)

		led_set_brightness(&app.ledr, 0)
		led_set_brightness(&app.ledg, 0)

		time.Sleep(time.Millisecond * 200)

		app.usage = proc_stat_get_cpu_usage(&app.stat)
		fmt.Printf("\rcpu usage = %d %%  ", app.usage)

		if app.usage < 30 {
			led_set_brightness(&app.ledg, 30)
		} else if app.usage < 60 {
			led_set_brightness(&app.ledg, 5)
		} else if app.usage < 90 {
			led_set_brightness(&app.ledr, 15)
		} else {
			led_set_brightness(&app.ledr, 150)
		}

		select {
			case <- app.running:
				break main_loop
			default:
		}
	}

	fmt.Printf("\n")
	led_fini(&app.ledr)
	led_fini(&app.ledg)
}