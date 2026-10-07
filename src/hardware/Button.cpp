#include "Button.h"

Button::Button(uint pin) : button_pin(pin) {
	gpio_init(button_pin);
	gpio_set_dir(button_pin, GPIO_IN);
	gpio_pull_up(button_pin);
	gpio_set_inover(button_pin, GPIO_OVERRIDE_INVERT); 
	held = gpio_get(button_pin); 
}

void Button::sample() {
	uint32_t now = to_ms_since_boot(get_absolute_time());
	if (now - last_ms >= BUTTON_POLL_MS) {
		last_ms = now;
		held = gpio_get(button_pin);
	}
}

bool Button::pressed() {
	bool before = held;
	sample();
	return held && !before;
}

bool Button::pressed_with(Button& other) {
	bool before = both_held;
	sample();
	other.sample();
	both_held = held && other.held;
	return both_held && !before;
}
