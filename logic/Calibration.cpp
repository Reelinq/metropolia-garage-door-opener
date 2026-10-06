#include <stdio.h>
#include "pico/stdlib.h"

#include "Calibration.h"

Calibration::Calibration(Door& door, Stepper& stepper, RotaryEncoder& encoder,
	const LimitSwitch& closed_sw, const LimitSwitch& open_sw)
	: door(door), stepper(stepper), encoder(encoder),
	closed_sw(closed_sw), open_sw(open_sw) {}

void Calibration::run_until_pressed(int dir, const LimitSwitch& sw) {
	encoder.read_ticks(); // discard old ticks
	encoder.watchdog_start(CALIBRATION_WD_MS);

	while (!sw.pressed()) {
		int t = encoder.read_ticks_watched(); // feeds the watchdog on movement
		if (t != 0) { door.on_movement(t); }
		stepper.step(dir);
	}

	stepper.off();
	encoder.watchdog_stop();
	// Count the last ticks from the belt settling
	sleep_ms(SETTLE_MS);
	int t = encoder.read_ticks();
	if (t != 0) { door.on_movement(t); }
}

bool Calibration::run() {
	printf("Calibration started\r\n");
	door.set_calibrated(false); 
	run_until_pressed(-1, closed_sw); // find the closed end
	door.set_position(0);
	run_until_pressed(+1, open_sw); // run to the open end
	int ticks = door.position();
	if (ticks > 0) {
		door.set_calibrated(true, ticks);
		printf("Calibrated: %d ticks between the limit switches\r\n", ticks);
		return true;
	}
	// Ticks counted the wrong way: encoder sign does not match the motor direction
	printf("Calibration failed: encoder counted %d ticks while opening\r\n", ticks);
	return false;
}
