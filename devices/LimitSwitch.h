#pragma once
#include "header.h"

class LimitSwitch {
	public:
		LimitSwitch(uint pin);
		bool pressed() const; // true when the switch is closed

	private:
		uint switch_pin;
};
