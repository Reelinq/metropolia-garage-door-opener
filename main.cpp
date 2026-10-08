#include "System.h"

int main() {
	stdio_init_all();
	sleep_ms(2000); // Give the USB console time to connect, so boot messages are not lost

	System sys;

	while (true) {
		// Buttons
		if (sys.sw0.pressed_with(sys.sw2)) {
			sys.controller.calibrate();
		}
		if (sys.sw1.pressed()) {
			sys.controller.toggle();
		}

		// Encoder, limit switches, motor step
		sys.controller.update();

		sys.remote.update();

		sys.leds.update(sys.door);
	}
}
