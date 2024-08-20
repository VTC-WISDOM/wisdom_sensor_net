#include "hardware/i2c.h"

#include "pcf8523_i2c_generic.h"

bool pcf8523_i2c_read(uint i2c_inst, uint8_t *buf, size_t buf_len) {
	bool success = false;

	int rval = i2c_read_blocking(i2c_get_instance(i2c_inst), PCF8523_I2C_ADDRESS, buf, buf_len, false);
	if (rval == buf_len) success = true;

	return success;
}
bool pcf8523_i2c_write(uint i2c_inst, uint8_t *buf, size_t buf_len) {
	bool success = false;

	int rval = i2c_write_blocking(i2c_get_instance(i2c_inst), PCF8523_I2C_ADDRESS, buf, buf_len, false);
	if (rval == buf_len) success = true;

	return success;
}
