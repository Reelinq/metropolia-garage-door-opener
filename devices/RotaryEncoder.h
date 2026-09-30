#pragma once

#include <stdint.h>
#include "pico/stdlib.h"
#include "pico/util/queue.h"

class RotaryEncoder {
	public:
		RotaryEncoder(uint a_pin, uint b_pin);
		RotaryEncoder(const RotaryEncoder&) = delete; // one object = one piece of hardware

		// Net ticks since the last call (+1 one way, -1 the other).
		int read_ticks();

	private:
		// Interrupt on rising edge of A. static: the SDK can't call normal members
		static void gpio_callback(uint gpio, uint32_t events);
		static RotaryEncoder* instance; // the callback needs a way to find the object

		uint rot_a_pin, rot_b_pin;
		queue_t rot_queue; // callback puts +1/-1 in, read_ticks() takes them out
};
