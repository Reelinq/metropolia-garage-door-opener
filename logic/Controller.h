#pragma once

#include "Door.h"
#include "Stepper.h"
#include "RotaryEncoder.h"
#include "LimitSwitch.h"
#include "Calibration.h"

// Max time the motor may run without a single encoder tick before the door counts as stuck.
// Moved here from main.cpp.
#define MOVE_WD_MS 2000

class Controller {
	public:
		Controller(Door& door, Stepper& stepper, RotaryEncoder& encoder,
			const LimitSwitch& closed_sw, const LimitSwitch& open_sw);

		void stop(); // motor off, watchdog off
		void toggle(); // SW1 behaviour: start, stop or reverse
		bool calibrate(); // blocking, returns true if calibrated afterwards
		void update(); // call every loop: encoder, limit switches, one motor step

	private:
		void start(int dir); // motor on, watchdog on
		void check_limits();

		Door& door;
		Stepper& stepper;
		RotaryEncoder& encoder;
		const LimitSwitch& closed_sw;
		const LimitSwitch& open_sw;
		Calibration calibration; // declared last: it is built from the references above
};
