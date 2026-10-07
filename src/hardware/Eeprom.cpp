#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

#include "Eeprom.h"

Eeprom::Eeprom(i2c_inst_t* port, uint sda_pin, uint scl_pin,
	uint8_t dev_addr, uint baud)
	: i2c(port), dev_addr(dev_addr) {

	i2c_init(i2c, baud);
	gpio_set_function(sda_pin, GPIO_FUNC_I2C);
	gpio_set_function(scl_pin, GPIO_FUNC_I2C);
	gpio_pull_up(sda_pin);
	gpio_pull_up(scl_pin);
}

bool Eeprom::write(uint16_t mem_addr, const uint8_t* data, size_t len) {
	if (len == 0 || len > EEPROM_MAX_WRITE) {
		return false;
	}
	// A write must stay inside one page, or the chip wraps around
	if ((mem_addr % EEPROM_PAGE_SIZE) + len > EEPROM_PAGE_SIZE) {
		return false;
	}

	uint8_t buf[2 + EEPROM_MAX_WRITE];
	buf[0] = (uint8_t)(mem_addr >> 8); // address high byte
	buf[1] = (uint8_t)(mem_addr & 0xFF); // address low byte
	for (size_t i = 0; i < len; i++) {
		buf[2 + i] = data[i];
	}

	int expected = (int)(2 + len);
	int written = i2c_write_blocking(i2c, dev_addr, buf, expected, false);
	if (written != expected) {
		return false;
	}

	sleep_ms(EEPROM_WRITE_MS); // chip is busy writing internally
	return true;
}

bool Eeprom::read(uint16_t mem_addr, uint8_t* data, size_t len) {
	if (len == 0) {
		return false;
	}

	uint8_t addr_buf[2] = { (uint8_t)(mem_addr >> 8), (uint8_t)(mem_addr & 0xFF) };

	// true = keep the bus (repeated start), so the address pointer is not lost
	if (i2c_write_blocking(i2c, dev_addr, addr_buf, 2, true) != 2) {
		return false;
	}
	return i2c_read_blocking(i2c, dev_addr, data, len, false) == (int)len;
}
