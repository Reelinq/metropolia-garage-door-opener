#include "StatusLeds.h"

StatusLeds::StatusLeds()
	: led_closed(LED0_PIN), led_open(LED1_PIN), led_status(LED2_PIN) {}

void StatusLeds::update(const Door& door) {
	DoorState s = door.state();
	led_closed.set_mode(s == DoorState::Closed ? LedMode::On : LedMode::Off);
	led_open.set_mode(s == DoorState::Open ? LedMode::On : LedMode::Off);

	// Check Stuck first: a stuck door is also not calibrated
	if (door.error() == DoorError::Stuck) {
		led_status.set_mode(LedMode::Blink);
	} else if (door.calibration() == CalibrationState::Calibrated) {
		led_status.set_mode(LedMode::On);
	} else {
		led_status.set_mode(LedMode::Off);
	}

	led_closed.update();
	led_open.update();
	led_status.update();
}
