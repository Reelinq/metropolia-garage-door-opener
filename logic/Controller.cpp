

#include "Controller.h"

Controller::Controller(Door& door, Stepper& stepper, RotaryEncoder& encoder,
	const LimitSwitch& closed_sw, const LimitSwitch& open_sw)
	: door(door), stepper(stepper), encoder(encoder),
	closed_sw(closed_sw), open_sw(open_sw),
	calibration(door, stepper, encoder, closed_sw, open_sw) {
	if (encoder.caused_stuck_reset()) {
		door.set_error(DoorError::Stuck); // also sets "not calibrated"
		printf("Stuck reset detected\r\n");
	}
}

void Controller::stop() {
	stepper.stop(); // pins off, direction 0
	encoder.watchdog_stop(); 
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
	return calibration.run();
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
		stop();
		door.set_position(door.total_ticks());
	} else if (dir < 0 && closed_sw.pressed()) {
		stop();
		door.set_position(0);
	}
}
