#include <stdio.h>
#include "pico/stdlib.h"

#include "Stepper.h"
#include "RotaryEncoder.h"
#include "LimitSwitch.h"
#include "Door.h"
#include "Button.h"

#define ROT_A_PIN 10
#define ROT_B_PIN 11

#define CLOSED_SW_PIN 27
#define OPEN_SW_PIN 28

#define SW0_PIN 7
#define SW1_PIN 8
#define SW2_PIN 9

#define STEPPER_IN1_PIN 2
#define STEPPER_IN2_PIN 3
#define STEPPER_IN3_PIN 6
#define STEPPER_IN4_PIN 13

int main() {
	stdio_init_all();
	sleep_ms(2000); //NOTE: Maybe not needed

	Stepper stepper(STEPPER_IN1_PIN, STEPPER_IN2_PIN, STEPPER_IN3_PIN, STEPPER_IN4_PIN);
	RotaryEncoder encoder(ROT_A_PIN, ROT_B_PIN);
	LimitSwitch closed_sw(CLOSED_SW_PIN);
	LimitSwitch open_sw(OPEN_SW_PIN);
	Door door;
	int dir = 0; // motor direction: +1 opening, -1 closing, 0 stopped

	Button sw0(SW0_PIN);
	Button sw1(SW1_PIN);
	Button sw2(SW2_PIN);

	// TODO: becomes Controller::stop()
	auto stop = [&]() {
		stepper.off();
		dir = 0;
	};

	while (true) {
		if (sw0.pressed_with(sw2)) {
			stop();

			// TODO: Class Calibration
			printf("Calibration started\r\n");
		}

		int ticks = encoder.read_ticks();
		if (ticks != 0) { door.on_movement(ticks); }

		// TODO: Could be shortened into a function?
		if (dir > 0 && open_sw.pressed()) {
			stop();
			door.set_position(door.total_ticks());
		}
		if (dir < 0 && closed_sw.pressed()) {
			stop();
			door.set_position(0);
		}

		// Move motor
		if (dir != 0) {
			stepper.step(dir);
		}

		// Local button SW1
		if (sw1.pressed()) {
			if (door.calibration() != CalibrationState::Calibrated ||
				dir != 0) {
				// Not calibrated, or pressed while moving -> stop
				stop();
			} else if (door.state() == DoorState::Closed) {
				dir = +1;
			} else if (door.state() == DoorState::Open) {
				dir = -1;
			}
			// TODO: stopped in between -> go the opposite way to the previous, (remember the last direction)?
		}

		// TODO: Stuck detection

		// TODO: Class Mqtt

		// TODO: Class Led
	}
}
