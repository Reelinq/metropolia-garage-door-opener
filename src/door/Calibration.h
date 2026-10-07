#pragma once

#include "Door.h"
#include "Stepper.h"
#include "RotaryEncoder.h"
#include "LimitSwitch.h"
#include "Config.h"


class Calibration {
	public:
		Calibration(Door& door, Stepper& stepper, RotaryEncoder& encoder,
			const LimitSwitch& closed_sw, const LimitSwitch& open_sw);
		// Blocking. Returns true if the door is calibrated afterwards.
		bool run();

	private:
		void run_until_pressed(int dir, const LimitSwitch& sw);

		Door& door;
		Stepper& stepper;
		RotaryEncoder& encoder;
		const LimitSwitch& closed_sw;
		const LimitSwitch& open_sw;
};
