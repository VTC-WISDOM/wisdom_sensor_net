#ifndef PCF8523_I2C_GENERIC_H
#define PCF8523_I2C_GENERIC_H

#include <stddef.h>

#define PCF8523_I2C_ADDRESS (0x68)

bool pcf8523_i2c_read(uint i2c_inst, uint8_t *buf, size_t buf_len);
bool pcf8523_i2c_write(uint i2c_inst, uint8_t *buf, size_t buf_len);

#endif // PCF8523_I2C_GENERIC_H
