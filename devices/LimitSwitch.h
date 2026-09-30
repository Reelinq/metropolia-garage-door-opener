#pragma once

#include "pico/stdlib.h"

class LimitSwitch {
	public:
		LimitSwitch(uint pin);
		bool pressed() const; // true when the switch is closed

	private:
		uint switch_pin;
};
