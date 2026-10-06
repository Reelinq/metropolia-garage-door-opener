#include <stdio.h>
#include "pico/stdlib.h"

#include "Controller.h"

Controller::Controller(Door& door, Stepper& stepper, RotaryEncoder& encoder,
	const LimitSwitch& closed_sw, const LimitSwitch& open_sw)
	: door(door), stepper(stepper), encoder(encoder),
	closed_sw(closed_sw), open_sw(open_sw),
	calibration(door, stepper, encoder, closed_sw, open_sw) {

	// Did the last reset come from a stuck door? Must run exactly once: it clears the marker.
	if (encoder.caused_stuck_reset()) {
		door.set_error(DoorError::Stuck); // also sets "not calibrated"
		printf("Stuck reset detected\r\n");
	}
}

void Controller::stop() {
	stepper.stop(); // pins off, direction 0
	encoder.watchdog_stop(); // harmless if it is not running
}

void Controller::start(int dir) {
	stepper.start(dir);
	encoder.watchdog_start(MOVE_WD_MS);
}

void Controller::toggle() {
	int d = door.next_direction(stepper.direction());
	if (d == 0) {
		stop();
	} else {
		start(d);
	}
}

bool Controller::calibrate() {
	stop(); // calibration drives the stepper itself
	return calibration.run();
}

void Controller::update() {
	// Always read, also when stopped: the belt coasts after a stop
	int ticks = encoder.read_ticks_watched();
	if (ticks != 0) { door.on_movement(ticks); }

	check_limits();

	stepper.update(); // one step if the direction is not 0
}

void Controller::check_limits() {
	int dir = stepper.direction();
	if (dir > 0 && open_sw.pressed()) {
		stop();
		door.set_position(door.total_ticks());
	} else if (dir < 0 && closed_sw.pressed()) {
		stop();
		door.set_position(0);
	}
}
