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

void Door::run_until_pressed(int dir, const LimitSwitch& sw, Stepper& stepper, RotaryEncoder& encoder) {
	encoder.read_ticks();                        // discard old ticks
	encoder.watchdog_start(CALIBRATION_WD_MS);
 
	while (!sw.pressed()) {
		int t = encoder.read_ticks_watched();    // feeds the watchdog on movement
		if (t != 0) { on_movement(t); }
		stepper.step(dir);
	}
 
	stepper.off();
	encoder.watchdog_stop();
 
	// Count the last ticks from the belt settling
	sleep_ms(SETTLE_MS);
	int t = encoder.read_ticks();
	if (t != 0) { on_movement(t); }
}
 
bool Door::calibrate(const LimitSwitch& closed_sw, const LimitSwitch& open_sw,
					 Stepper& stepper, RotaryEncoder& encoder) {
	printf("Calibration started\r\n");
	set_calibrated(false);                       // position is unknown until finished
 
	run_until_pressed(-1, closed_sw, stepper, encoder); // find the closed end
	set_position(0);
 
	run_until_pressed(+1, open_sw, stepper, encoder);   // run to the open end
	int ticks = position();
 
	if (ticks > 0) {
		set_calibrated(true, ticks);
		printf("Calibrated: %d ticks between the limit switches\r\n", ticks);
		return true;
	}
 
	// Ticks counted the wrong way: encoder sign does not match the motor direction
	printf("Calibration failed: encoder counted %d ticks while opening\r\n", ticks);
	return false;
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
