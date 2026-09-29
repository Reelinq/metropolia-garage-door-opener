#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "Stepper.h"

// Constructor: copies the pins into the object and sets them up.
Stepper::Stepper(uint8_t in1, uint8_t in2, uint8_t in3, uint8_t in4)
	: motor_pins{in1, in2, in3, in4}, step_index(0) {

	for (uint8_t i = 0; i < MOTOR_PIN_COUNT; i++) {
		gpio_init(motor_pins[i]);
		gpio_set_dir(motor_pins[i], GPIO_OUT);
		gpio_put(motor_pins[i], 0);
	}
}

void Stepper::off() {
	const uint8_t ALL_OFF[MOTOR_PIN_COUNT] = {0, 0, 0, 0};
	motor_set(ALL_OFF);
}

void Stepper::step(int dir) {
	// Go to the next row (or the previous one), wrapping around the table
	if (dir > 0) {
		step_index = (step_index + 1) % HALF_STEP_COUNT;
	} else {
		step_index = (step_index + HALF_STEP_COUNT - 1) % HALF_STEP_COUNT;
	}

	motor_set(HALF_STEP[step_index]);
	sleep_ms(1); // Motor needs time to physically move
}

void Stepper::motor_set(const uint8_t levels[MOTOR_PIN_COUNT]) {
	for (uint8_t i = 0; i < MOTOR_PIN_COUNT; i++) {
		gpio_put(motor_pins[i], levels[i]);
	}
}
