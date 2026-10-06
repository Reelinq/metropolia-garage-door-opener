#pragma once

#include "Door.h"
#include "Stepper.h"
#include "RotaryEncoder.h"
#include "LimitSwitch.h"

// Calibration: if the encoder gives no movement for this long while the motor
// runs and the limit switch has not closed, the door is stuck (watchdog resets the chip).
#define CALIBRATION_WD_MS 2000

// Time for the belt to settle after the motor is switched off, before reading
// the last encoder ticks.
#define SETTLE_MS 100

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
