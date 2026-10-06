#include "Stepper.h"
#include "RotaryEncoder.h"
#include "LimitSwitch.h"
#include "Door.h"
#include "Controller.h"
#include "Button.h"
#include "Led.h"
#include "Eeprom.h"
#include "Mqtt.h"
#include "RemoteControl.h"

#define EEPROM_SDA_PIN 16
#define EEPROM_SCL_PIN 17
#define EEPROM_I2C_ADDR 0x50

int main() {
	stdio_init_all();
	sleep_ms(2000); // Give the USB console time to connect, so boot messages are not lost

	Stepper stepper(STEPPER_IN1_PIN, STEPPER_IN2_PIN, STEPPER_IN3_PIN, STEPPER_IN4_PIN);
	RotaryEncoder encoder(ROT_A_PIN, ROT_B_PIN);
	LimitSwitch closed_sw(CLOSED_SW_PIN);
	LimitSwitch open_sw(OPEN_SW_PIN);
	Door door;
	Eeprom eeprom(i2c0, EEPROM_SDA_PIN, EEPROM_SCL_PIN, EEPROM_I2C_ADDR);
	Mqtt mqtt(WIFI_SSID, WIFI_PASSWORD, BROKER_IP, BROKER_PORT);
	Controller controller(door, stepper, encoder, closed_sw, open_sw, eeprom);
	RemoteControl remote(mqtt, door, controller);

	Button sw0(SW0_PIN);
	Button sw1(SW1_PIN);
	Button sw2(SW2_PIN);

	Led led_closed(LED0_PIN);
	Led led_open(LED1_PIN);
	Led led_status(LED2_PIN);

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

	update_leds();

	while (true) {
		// Buttons
		if (sw0.pressed_with(sw2)) {
			controller.calibrate();
		}
		if (sw1.pressed()) {
			controller.toggle();
		}

		// Encoder, limit switches, motor step
		controller.update();

		remote.update();

		update_leds();
	}
}
