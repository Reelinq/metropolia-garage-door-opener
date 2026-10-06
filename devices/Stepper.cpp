#include "Stepper.h"

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
	if (dir > 0) {
		step_index = (step_index + 1) % HALF_STEP_COUNT;
	} else {
		step_index = (step_index + HALF_STEP_COUNT - 1) % HALF_STEP_COUNT;
	}

	motor_set(HALF_STEP[step_index]);
	sleep_ms(1); 
}

void Stepper::motor_set(const uint8_t levels[MOTOR_PIN_COUNT]) {
	for (uint8_t i = 0; i < MOTOR_PIN_COUNT; i++) {
		gpio_put(motor_pins[i], levels[i]);
	}
}

void Stepper::start(int dir) {
	if (dir == 0) {
		stop();
		return;
	}
	move_dir = (dir > 0) ? +1 : -1;
}

void Stepper::stop() {
	off();
	move_dir = 0;
}
 
void Stepper::update() {
	if (move_dir != 0) {
		step(move_dir);
	}
}
 
int Stepper::direction() const { return move_dir; }
bool Stepper::moving() const { return move_dir != 0; }

