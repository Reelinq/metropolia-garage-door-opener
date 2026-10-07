#pragma once
#include "Config.h"

enum class LedMode { Off, On, Blink };

class Led {
public:
	Led(uint pin);
	void set_mode(LedMode m);
	void update();
private:
	uint led_pin;
	LedMode mode = LedMode::Off;
	bool lit = false;
	uint32_t last_toggle_ms = 0;
};
