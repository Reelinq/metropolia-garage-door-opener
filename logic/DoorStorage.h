#pragma once

#include <stdint.h>

#include "Eeprom.h"
#include "Door.h"

#define STORAGE_ADDR 0x0000 // 6 bytes, fits inside page 0

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
