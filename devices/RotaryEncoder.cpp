#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/watchdog.h"

#include "RotaryEncoder.h"

// Max ticks waiting between two read_ticks() calls (more are lost).
#define ROT_QUEUE_SIZE 10

// A static variable must be defined once, outside the class
RotaryEncoder* RotaryEncoder::instance = nullptr;

RotaryEncoder::RotaryEncoder(uint a_pin, uint b_pin)
	: rot_a_pin(a_pin), rot_b_pin(b_pin) {

	queue_init(&rot_queue, sizeof(int), ROT_QUEUE_SIZE);

	// Inputs without internal pulls (the board has external pull-ups)
	gpio_init(rot_a_pin);
	gpio_set_dir(rot_a_pin, GPIO_IN);
	gpio_disable_pulls(rot_a_pin); //NOTE: maybe not needed

	gpio_init(rot_b_pin);
	gpio_set_dir(rot_b_pin, GPIO_IN);
	gpio_disable_pulls(rot_b_pin); //NOTE: maybe not needed

	// This is set before enabling the interrupt, because the callback uses it
	instance = this;

	gpio_set_irq_enabled_with_callback(
		rot_a_pin,
		GPIO_IRQ_EDGE_RISE,
		true,
		&gpio_callback
	);
}

void RotaryEncoder::gpio_callback(uint gpio, uint32_t /*events*/) {
	// Ignore other pins and calls before the encoder exists
	if (instance == nullptr || gpio != instance->rot_a_pin) {
		return;
	}

	// B low on rising A = clockwise (+1), B high = counter-clockwise (-1)
	int dir = gpio_get(instance->rot_b_pin) ? -1 : +1;
	queue_try_add(&instance->rot_queue, &dir); // drops if queue is full
}

int RotaryEncoder::read_ticks() {
	int ticks = 0;
	int dir;

	while (queue_try_remove(&rot_queue, &dir)) {
		ticks += dir;
	}
	return ticks;
}

void RotaryEncoder::watchdog_start(uint32_t timeout_ms) {
	watchdog_hw->scratch[0] = WD_MOVING_MAGIC; // "reset while moving" = stuck
	watchdog_enable(timeout_ms, true);         // true = pause while debugging
	wd_active = true;
}
 
void RotaryEncoder::watchdog_stop() {
	// The SDK has no disable function; clearing the enable bit stops the countdown.
	hw_clear_bits(&watchdog_hw->ctrl, WATCHDOG_CTRL_ENABLE_BITS);
	watchdog_hw->scratch[0] = 0;
	wd_active = false;
}
 
bool RotaryEncoder::watchdog_running() const { return wd_active; }
 
int RotaryEncoder::read_ticks_watched() {
	int ticks = read_ticks();
	if (wd_active && ticks != 0) {
		watchdog_update(); // encoder moved -> door is not stuck
	}
	return ticks;
}
 
bool RotaryEncoder::caused_stuck_reset() {
	bool stuck = watchdog_enable_caused_reboot() && watchdog_hw->scratch[0] == WD_MOVING_MAGIC;
	watchdog_hw->scratch[0] = 0;
	return stuck;
}

