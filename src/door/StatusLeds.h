#pragma once

#include "Config.h"
#include "Led.h"
#include "Door.h"

// Maps the door state to the three LEDs
class StatusLeds {
public:
	StatusLeds();
	void update(const Door& door); // call every loop

private:
	Led led_closed;
	Led led_open;
	Led led_status;
};
