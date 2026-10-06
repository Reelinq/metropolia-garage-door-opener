#pragma once

#include <stdint.h>
#include <stddef.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "header.h"

class Eeprom {
	public:
		// Sets up the I2C bus and pins itself
		Eeprom(i2c_inst_t* port, uint sda_pin, uint scl_pin,
			uint8_t dev_addr, uint baud = 100000);

		// Both return true only if every byte was transferred
		bool read(uint16_t mem_addr, uint8_t* data, size_t len);
		bool write(uint16_t mem_addr, const uint8_t* data, size_t len);

	private:
		i2c_inst_t* i2c;
		uint8_t dev_addr;
};
