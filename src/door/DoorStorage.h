#pragma once

#include <stdint.h>

#include "Eeprom.h"
#include "Door.h"

class DoorStorage {
	public:
		DoorStorage(Eeprom& eeprom);

		// Saves calibration and position. A door that is not calibrated is saved as total = 0.
		bool save(const Door& door);

		// Restores the door. Returns false if the record is bad or the door was not calibrated.
		bool load(Door& door);

	private:
		Eeprom& eeprom;
};
