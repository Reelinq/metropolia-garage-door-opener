#pragma once
#include "Config.h"

class RotaryEncoder {
	public:
		RotaryEncoder(uint a_pin, uint b_pin);
		RotaryEncoder(const RotaryEncoder&) = delete;

		// Net ticks since the last call (+1 one way, -1 the other).
		int read_ticks();
		void watchdog_start(uint32_t timeout_ms = 2000); // call when the motor starts
		void watchdog_stop(); // call when the motor stops normally
		bool watchdog_running() const;
		// Same as read_ticks(), but also feeds the watchdog if movement was seen.
		int read_ticks_watched();
		static bool caused_stuck_reset();
	private:
		static void gpio_callback(uint gpio, uint32_t events);
		static RotaryEncoder* instance;
		bool wd_active = false;


		uint rot_a_pin, rot_b_pin;
		queue_t rot_queue;
};
