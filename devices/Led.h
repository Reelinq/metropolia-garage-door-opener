#pragma once
#include "header.h"

enum class LedMode { Off, On, Blink };

class Led {
public:
	Led(uint pin);

	// Safe to call every loop: nothing happens unless the mode actually changes
	void set_mode(LedMode m);

	// Call every loop: toggles the pin once the blink time has passed
	void update();

private:
	uint led_pin;
	LedMode mode = LedMode::Off;
	bool lit = false;
	uint32_t last_toggle_ms = 0;
};
