#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "led.h"

Led::Led(uint pin) : led_pin(pin) {
	gpio_init(led_pin);
	gpio_set_dir(led_pin, GPIO_OUT);
	gpio_put(led_pin, 0); // starts off, matches mode = Off
}

void Led::set_mode(LedMode m) {
	if (m == mode) {
		return; // same mode: do not restart the blink phase
	}
	mode = m;
	last_toggle_ms = to_ms_since_boot(get_absolute_time());
	lit = (mode != LedMode::Off); // On and Blink both start lit
	gpio_put(led_pin, lit);
}

void Led::update() {
	if (mode != LedMode::Blink) {
		return; // Off and On need no timing
	}
	uint32_t now = to_ms_since_boot(get_absolute_time());
	if (now - last_toggle_ms >= LED_BLINK_MS) {
		last_toggle_ms = now;
		lit = !lit;
		gpio_put(led_pin, lit);
	}
}
