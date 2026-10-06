#include <stdio.h>
#include "pico/stdlib.h"

#include "Stepper.h"
#include "RotaryEncoder.h"
#include "LimitSwitch.h"
#include "Door.h"
#include "Calibration.h"
#include "Button.h"
#include "led.h"

#define LED0_PIN 20
#define LED1_PIN 21
#define LED2_PIN 22

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
	Calibration calibration(door, stepper, encoder, closed_sw, open_sw);
	int dir = 0; // motor direction: +1 opening, -1 closing, 0 stopped

	Button sw0(SW0_PIN);
	Button sw1(SW1_PIN);
	Button sw2(SW2_PIN);

	Led led_closed(LED0_PIN);
	Led led_open(LED1_PIN);
	Led led_status(LED2_PIN);

	// TODO: becomes Controller::stop()
	auto stop = [&]() {
		stepper.off();
		dir = 0;
	};

	// Maps door state to the three LEDs. Called every loop.
	auto update_leds = [&]() {
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
	};

	// Stops at the end switch in the direction of travel and re-syncs the position
	auto check_limits = [&]() {
		if (dir > 0 && open_sw.pressed()) {
			stop();
			door.set_position(door.total_ticks());
		} else if (dir < 0 && closed_sw.pressed()) {
			stop();
			door.set_position(0);
		}
	};

	update_leds();

	while (true) {
		// 1. Buttons
		if (sw0.pressed_with(sw2)) {
			stop();
			calibration.run();
		}

		if (sw1.pressed()) {
			dir = door.next_direction(dir);
			if (dir == 0) {
				stop();
			}
		}

		// 2. Encoder: always read, also when stopped (the belt coasts after a stop)
		int ticks = encoder.read_ticks();
		if (ticks != 0) { door.on_movement(ticks); }

		// 3. Limit switches
		check_limits();

		// 4. Move motor (one step, about 1 ms)
		if (dir != 0) {
			stepper.step(dir);
		}

		// 5. Report state changes, only when they happen
		DoorState now = door.state();
		if (now != last_state) {
			printf("Door: %s\r\n", state_name(now));
			last_state = now;
		}

		// TODO: Stuck detection

		// TODO: Class Mqtt

		update_leds();
	}
}
