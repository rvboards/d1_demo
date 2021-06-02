package main

import "fmt"
import "os"


type led_t struct {
	devpath string
	brightness uint8
}


func led_init(led *led_t, devpath string) {
	led.devpath = devpath

	led_set_brightness(led, 0)
}


func led_fini(led *led_t) {
	led_set_brightness(led, 0)
}


func led_set_brightness(led *led_t, brightness uint8) {
	file, err := os.OpenFile(led.devpath, os.O_WRONLY, 0664)
	if err != nil {
		panic(err)
	}

	fmt.Fprintf(file, "%d\n", int(brightness))

	err = file.Close()
	if err != nil {
		panic(err)
	}

	led.brightness = brightness
}