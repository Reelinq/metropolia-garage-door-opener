#pragma once
#include "header.h"

class LimitSwitch {
	public:
		LimitSwitch(uint pin);
		bool pressed() const;

	private:
		uint switch_pin;
};
