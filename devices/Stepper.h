#pragma once
#include "header.h"

// Each row has MOTOR_PIN_COUNT columns (one for each pin).
static const uint8_t HALF_STEP[HALF_STEP_COUNT][MOTOR_PIN_COUNT] = {
	{1, 0, 0, 0},
	{1, 1, 0, 0},
	{0, 1, 0, 0},
	{0, 1, 1, 0},
	{0, 0, 1, 0},
	{0, 0, 1, 1},
	{0, 0, 0, 1},
	{1, 0, 0, 1},
};

class Stepper {
	public:
		Stepper(uint8_t in1, uint8_t in2, uint8_t in3, uint8_t in4);
		void step(int dir);
		void off();
		// ---- Movement state (door opening / closing / stopped) ----
		void start(int dir);   // +1 opening, -1 closing (0 = stop)
		void stop();           // coils off, direction = 0
		void update();         // call every loop: takes one step while moving
		int direction() const; // +1 opening, -1 closing, 0 stopped
		bool moving() const;


	private:
		void motor_set(const uint8_t levels[MOTOR_PIN_COUNT]);
		uint8_t motor_pins[MOTOR_PIN_COUNT];
		uint8_t step_index;
		int move_dir = 0; // direction of the current movement
};
