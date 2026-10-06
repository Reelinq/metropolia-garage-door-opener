#include "pico/stdlib.h"

#include "Door.h"

void Door::on_movement(int ticks) { pos += ticks; }
void Door::set_position(int ticks) { pos = ticks; }

void Door::set_calibrated(bool calibrated, int ticks) {
	if (calibrated) {
		cal = CalibrationState::Calibrated;
		total = ticks;
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

int Door::next_direction(int current_dir) {
	if (cal != CalibrationState::Calibrated) {
		return 0;                      // not calibrated: door may not be moved
	}
	if (current_dir != 0) {
		last_dir = current_dir;        // remember which way it was going
		return 0;                      // moving -> stop
	}

	switch (state()) {
		case DoorState::Closed:
			last_dir = +1;
			break;
		case DoorState::Open:
			last_dir = -1;
			break;
		default:                       // stopped midway: go the opposite way
			last_dir = (last_dir == 0) ? +1 : -last_dir;
			break;
	}
	return last_dir;
}


DoorState Door::state() const {
	if (cal != CalibrationState::Calibrated) {
		return DoorState::InBetween; // position unknown
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
int Door::total_ticks() const { return total; }
