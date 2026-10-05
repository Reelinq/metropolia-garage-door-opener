#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "led.h"

Led::Led(uint pin) : led_pin(pin) {
	gpio_init(led_pin);
	gpio_set_dir(led_pin, GPIO_OUT);
}

//true sets error, false sets normal state
void Led::SetError (bool error) {
	errorstate = error;
	blinkcounter = 0;
}

//turns the led on if not error, if error starts blinking
void Led::LedUpdate() {
	if (errorstate) {
		if (blinkcounter >= BLINKMAX) {
			gpio_put(led_pin, !gpio_get(led_pin));
			blinkcounter = 0;
		} else {
			++blinkcounter;
		}
	} else {
		gpio_put(led_pin, true);
	}
}
