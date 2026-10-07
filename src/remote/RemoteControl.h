#pragma once

#include "Mqtt.h"
#include "Door.h"
#include "Controller.h"

// Glue between MQTT and the door
class RemoteControl {
public:
	RemoteControl(Mqtt& mqtt, const Door& door, Controller& controller);
	void update(); // call every loop, must never block (except during calibrate)

private:
	Mqtt& mqtt;
	const Door& door;
	Controller& controller;
	char last_status[128] = ""; // last status JSON that was successfully published

	void handle_command();
	void publish_status();
};
