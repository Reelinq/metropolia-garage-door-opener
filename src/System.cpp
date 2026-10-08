#include "System.h"

System::System()
	: stepper(STEPPER_IN1_PIN, STEPPER_IN2_PIN, STEPPER_IN3_PIN, STEPPER_IN4_PIN),
	encoder(ROT_A_PIN, ROT_B_PIN),
	closed_sw(CLOSED_SW_PIN),
	open_sw(OPEN_SW_PIN),
	eeprom(i2c0, EEPROM_SDA_PIN, EEPROM_SCL_PIN, EEPROM_I2C_ADDR),
	mqtt(WIFI_SSID, WIFI_PASSWORD, BROKER_IP, BROKER_PORT),
	controller(door, stepper, encoder, closed_sw, open_sw, eeprom),
	remote(mqtt, door, controller),
	sw0(SW0_PIN), sw1(SW1_PIN), sw2(SW2_PIN) {}
