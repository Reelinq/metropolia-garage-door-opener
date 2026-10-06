#pragma once
#include "header.h"

class RotaryEncoder {
	public:
		RotaryEncoder(uint a_pin, uint b_pin);
		RotaryEncoder(const RotaryEncoder&) = delete; // one object = one piece of hardware

		// Net ticks since the last call (+1 one way, -1 the other).
		int read_ticks();

			// ---- Door stuck detection (RP2040 hardware watchdog) ----
		// Armed only while the motor should be moving. Fed whenever the encoder
		// reports movement. No movement for timeout_ms -> chip resets.
		// RP2040 max timeout is about 8388 ms.
		void watchdog_start(uint32_t timeout_ms = 2000); // call when the motor starts
		void watchdog_stop();                            // call when the motor stops normally
		bool watchdog_running() const;
 
		// Same as read_ticks(), but also feeds the watchdog if movement was seen.
		int read_ticks_watched();
 
		// Call ONCE at boot: true if the last reset was a stuck-door watchdog reset.
		static bool caused_stuck_reset();


	private:
		// Interrupt on rising edge of A. static: the SDK can't call normal members
		static void gpio_callback(uint gpio, uint32_t events);
		static RotaryEncoder* instance; // the callback needs a way to find the object
		bool wd_active = false;


		uint rot_a_pin, rot_b_pin;
		queue_t rot_queue; // callback puts +1/-1 in, read_ticks() takes them out
};
