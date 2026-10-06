

#include "Controller.h"

Controller::Controller(Door& door, Stepper& stepper, RotaryEncoder& encoder,
	const LimitSwitch& closed_sw, const LimitSwitch& open_sw, Eeprom& eeprom)
	: door(door), stepper(stepper), encoder(encoder), closed_sw(closed_sw), open_sw(open_sw),
	storage(eeprom), calibration(door, stepper, encoder, closed_sw, open_sw) {

	storage.load(door);

	// Did the last reset come from a stuck door? Must run exactly once: it clears the marker.
	if (encoder.caused_stuck_reset()) {
		door.set_error(DoorError::Stuck); // also sets "not calibrated"
		storage.save(door);
		printf("Stuck reset detected\r\n");
	}
}

void Controller::stop() {
	bool was_moving = stepper.moving();
	stepper.stop();
	encoder.watchdog_stop();
	if (was_moving) {
		storage.save(door);
	}
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
	stop();
	bool ok = calibration.run();
	storage.save(door); // calibrated on success, "not calibrated" on failure
	return ok;
}

void Controller::update() {
	int ticks = encoder.read_ticks_watched();
	if (ticks != 0) { door.on_movement(ticks); }
	check_limits();
	stepper.update();
}

void Controller::check_limits() {
	int dir = stepper.direction();
	if (dir > 0 && open_sw.pressed()) {
		door.set_position(door.total_ticks());
		stop();
	} else if (dir < 0 && closed_sw.pressed()) {
		door.set_position(0);
		stop();
	}
}
