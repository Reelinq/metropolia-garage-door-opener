#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "LimitSwitch.h"

LimitSwitch::LimitSwitch(uint pin) : switch_pin(pin) {
	gpio_init(switch_pin);
	gpio_set_dir(switch_pin, GPIO_IN);
	gpio_pull_up(switch_pin);
}

bool LimitSwitch::pressed() const {
	return !gpio_get(switch_pin);
}
