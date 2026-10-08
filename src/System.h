#pragma once

#include "Config.h"
#include "Stepper.h"
#include "RotaryEncoder.h"
#include "LimitSwitch.h"
#include "Eeprom.h"
#include "Button.h"
#include "Door.h"
#include "Controller.h"
#include "StatusLeds.h"
#include "Mqtt.h"
#include "RemoteControl.h"

// Creates and owns all the objects of the application
class System {
	public:
		System();
		Stepper stepper;
		RotaryEncoder encoder;
		LimitSwitch closed_sw;
		LimitSwitch open_sw;
		Door door;
		Eeprom eeprom;
		Mqtt mqtt;
		Controller controller;
		RemoteControl remote;
		Button sw0;
		Button sw1;
		Button sw2;
		StatusLeds leds;
};
