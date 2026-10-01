#include "Door.h"

void Door::on_motor_step(int dir) { pos += dir; }
void Door::set_position(int steps) { pos = steps;}

void Door::set_calibrated(bool calibrated, int steps) {
	if (calibrated) {
		cal = CalibrationState::Calibrated;
		total = steps;
		err = DoorError::Normal;
	} else {
		cal = CalibrationState::NotCalibrated;
		total = 0;
	}
}

void Door::set_error(DoorError e) {
	err = e;
	if (e == DoorError::Stuck) {
		set_calibrated(false);
	}
}

DoorState Door::state() const {
	if (cal != CalibrationState::Calibrated) {
		return DoorState::InBetween;
	}
	if (pos <= 0) {
		return DoorState::Closed;
	}
	if (pos >= total) {
		return DoorState::Open;
	}
	return DoorState::InBetween;
}

DoorError Door::error() const { return err; }
CalibrationState Door::calibration() const { return cal; }
int Door::position() const { return pos; }
int Door::total_steps() const { return total; }
