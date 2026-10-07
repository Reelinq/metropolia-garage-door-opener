#include <stdio.h>
#include <string.h>

#include "Mqtt.h"
#include "RemoteControl.h"

static const char* state_name(DoorState s) {
	return s == DoorState::Closed ? "Closed" : s == DoorState::Open ? "Open" : "In between";
}

RemoteControl::RemoteControl(Mqtt& mqtt, const Door& door, Controller& controller)
	: mqtt(mqtt), door(door), controller(controller) {
	if (mqtt.init()) { mqtt.subscribe(MQTT_TOPIC_COMMAND); }
}

void RemoteControl::update() {
	handle_command();
	publish_status();
}

// Same operations as the local buttons: calibrate (SW0+SW2) and toggle (SW1)
void RemoteControl::handle_command() {
	const char* cmd = mqtt.checkMsg(); // also keeps the connection alive
	if (!cmd) { return; }

	const char* err = nullptr;
	if (strcmp(cmd, "calibrate") == 0) {
		if (!controller.calibrate()) { err = "Calibration failed"; }
	} else if (strcmp(cmd, "toggle") == 0) {
		if (door.calibration() != CalibrationState::Calibrated) {
			err = "Not calibrated";
		} else {
			controller.toggle();
		}
	} else {
		err = "Unknown command";
	}

	char resp[64];
	snprintf(resp, sizeof(resp), err ? "{\"error\":\"%s\"}" : "{\"result\":\"ok\"}", err);
	mqtt.publish(MQTT_TOPIC_RESPONSE, resp);
}

// Publishes door state, error state and calibration state in one message,
// only when it differs from the last successfully published one.
void RemoteControl::publish_status() {
	static DoorState last_state = DoorState::InBetween;
	DoorState now = door.state();
	if (now != last_state) {
		printf("Door: %s\r\n", state_name(now));
		last_state = now;
	}

	char msg[sizeof(last_status)];
	snprintf(msg, sizeof(msg),
		"{\"door\":\"%s\",\"error\":\"%s\",\"calibration\":\"%s\"}",
		state_name(door.state()),
		door.error() == DoorError::Stuck ? "Door stuck" : "Normal",
		door.calibration() == CalibrationState::Calibrated ? "Calibrated" : "Not calibrated");

	if (strcmp(msg, last_status) == 0) {
		return;
	}
	if (mqtt.publish(MQTT_TOPIC_STATUS, msg)) { // on failure, retried next loop
		snprintf(last_status, sizeof(last_status), "%s", msg);
	}
}
