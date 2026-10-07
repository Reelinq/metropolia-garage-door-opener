#include <stdio.h>
#include <stdint.h>

#include "DoorStorage.h"

// Stored inverted, so a blank (0xFF) or all-zero EEPROM never passes
static uint16_t check_value(uint16_t total, uint16_t pos) {
	return (uint16_t)~(total + pos);
}

DoorStorage::DoorStorage(Eeprom& eeprom) : eeprom(eeprom) {}

bool DoorStorage::save(const Door& door) {
	uint16_t total = 0;
	uint16_t pos = 0;

	if (door.calibration() == CalibrationState::Calibrated) {
		int p = door.position();
		if (p < 0) { p = 0; } // the belt coasts past the ends
		if (p > door.total_ticks()) { p = door.total_ticks(); }
		total = (uint16_t)door.total_ticks();
		pos = (uint16_t)p;
	}

	uint16_t check = check_value(total, pos);
	uint8_t buf[STORAGE_RECORD_SIZE] = {
		(uint8_t)(total >> 8), (uint8_t)(total & 0xFF),
		(uint8_t)(pos >> 8),   (uint8_t)(pos & 0xFF),
		(uint8_t)(check >> 8), (uint8_t)(check & 0xFF)
	};

	if (!eeprom.write(STORAGE_ADDR, buf, STORAGE_RECORD_SIZE)) {
		printf("Storage: EEPROM write failed\r\n");
		return false;
	}
	return true;
}

bool DoorStorage::load(Door& door) {
	uint8_t buf[STORAGE_RECORD_SIZE];

	if (!eeprom.read(STORAGE_ADDR, buf, STORAGE_RECORD_SIZE)) {
		printf("Storage: EEPROM read failed\r\n");
		return false;
	}

	uint16_t total = (uint16_t)((buf[0] << 8) | buf[1]);
	uint16_t pos   = (uint16_t)((buf[2] << 8) | buf[3]);
	uint16_t check = (uint16_t)((buf[4] << 8) | buf[5]);

	if (check != check_value(total, pos)) {
		printf("Storage: no valid record\r\n");
		return false;
	}

	// total = 0 (saved as not calibrated) and pos > total are rejected by restore()
	if (!door.restore(pos, total)) {
		printf("Storage: door was not calibrated\r\n");
		return false;
	}
	printf("Storage: restored pos=%d total=%d\r\n", (int)pos, (int)total);
	return true;
}
