#pragma once

#include <stdint.h>
#include "pico/stdlib.h"

// Button pins (active low, internal pull-up)
#define SW0_PIN 7
#define SW1_PIN 8
#define SW2_PIN 9

// The pin is read at most once per this time. Works as the debounce,
// like polling the pin every few milliseconds.
#define BUTTON_POLL_MS 20

class Button {
	public:
		Button(uint pin);

		// true ONCE per press (not repeated while held down)
		bool pressed();

		// true ONCE when both buttons become pressed together (SW0 + SW2 = calibration)
		bool pressed_with(Button& other);

	private:
		void sample(); // reads the pin, at most once per BUTTON_POLL_MS

		uint button_pin;
		bool held = false;      // last sampled state, true = pressed
		bool both_held = false; // used by pressed_with()
		uint32_t last_ms = 0;
};
