#pragma once

#include "pico/stdlib.h"

#define CLOSED_SW_PIN 27
#define OPEN_SW_PIN 28


class LimitSwitch {
	public:
		LimitSwitch(uint pin);
		bool pressed() const; // true when the switch is closed

	private:
		uint switch_pin;
};
