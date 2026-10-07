#pragma once

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdint.h>
#include <stdio.h>
#include "pico/util/queue.h"
#include "hardware/watchdog.h"

// ---------- Led defines ----------

#define LED0_PIN 20
#define LED1_PIN 21
#define LED2_PIN 22
#define LED_BLINK_MS 250 // time between toggles, so one full blink takes 500 ms



// ---------- Button defines ----------

// Button pins (active low, internal pull-up)
#define SW0_PIN 7
#define SW1_PIN 8
#define SW2_PIN 9

// The pin is read at most once per this time. Works as the debounce,
// like polling the pin every few milliseconds.
#define BUTTON_POLL_MS 20



// ---------- LimitSwitch pin defines ----------

#define CLOSED_SW_PIN 27
#define OPEN_SW_PIN 28



// ---------- Watchdog defines ----------

// Marker written to watchdog scratch[0] while the motor is moving. If the chip
// resets from the watchdog and this value is still there, the door was stuck.
// Any value that is unlikely to appear by accident works (this one is "WDMV" in ASCII).
#define WD_MOVING_MAGIC 0x57444D56u



// ---------- Rotary defines ----------

// Max ticks waiting between two read_ticks() calls (more are lost).
#define ROT_QUEUE_SIZE 10

// Max time the motor may run without a single encoder tick before the door counts as stuck.
#define MOVE_WD_MS 2000



// ---------- Stepper defines ----------

// Stepper motor driver pins (IN1..IN4)
#define STEPPER_IN1_PIN 2
#define STEPPER_IN2_PIN 3
#define STEPPER_IN3_PIN 6
#define STEPPER_IN4_PIN 13

// Number of rows in the HALF_STEP table below.
#define HALF_STEP_COUNT 8

// Number of driver inputs (IN1..IN4), matching HALF_STEP table columns.
#define MOTOR_PIN_COUNT 4

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



// ---------- Calibration defines ----------

// Calibration: if the encoder gives no movement for this long while the motor
// runs and the limit switch has not closed, the door is stuck (watchdog resets the chip).
#define CALIBRATION_WD_MS 2000

// Time for the belt to settle after the motor is switched off, before reading
// the last encoder ticks.
#define SETTLE_MS 100

#define ROT_A_PIN 4
#define ROT_B_PIN 5



// ---------- Eeprom defines ----------

#define EEPROM_SDA_PIN 16
#define EEPROM_SCL_PIN 17
#define EEPROM_I2C_ADDR 0x50
#define EEPROM_PAGE_SIZE 64
#define EEPROM_MAX_WRITE 32
#define EEPROM_WRITE_MS 5
#define STORAGE_ADDR 0x0000 // 8 bytes, fits inside page 0
#define STORAGE_RECORD_SIZE 8



// ---------- MQTT defines ----------

#define BROKER_IP "10.161.4.63"
#define BROKER_PORT 1883
#define MQTT_TOPIC_COMMAND "garage/door/command"
#define MQTT_TOPIC_STATUS "garage/door/status"
#define MQTT_TOPIC_RESPONSE "garage/door/response"
