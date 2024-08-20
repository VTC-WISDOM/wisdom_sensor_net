#include <stdlib.h>

#include "sht30_rp2040.h"
#include "sht30_constants.h"

const uint8_t sht30_command_lookup[SHT30_COMMANDS_MAX][2] = {
	[SHT30_SSDA_CS_HIGH] = {0x2C, 0x06},
	[SHT30_SSDA_CS_MED] = {0x2C, 0x0D},
	[SHT30_SSDA_CS_LOW] = {0x2C, 0x10},
	[SHT30_SSDA_HIGH] = {0x24, 0x00},
	[SHT30_SSDA_MED] = {0x24, 0x0B},
	[SHT30_SSDA_LOW] = {0x24, 0x16},
};

struct _sht30_rp2040_s {
	i2c_inst_t *i2c;
	uint address;
	uint pin_scl;	
	uint pin_sda;
};

sht30_rp2040_t * sht30_rp2040_create(void) {
	return malloc(sizeof (struct _sht30_rp2040_s));
}

void sht30_rp2040_destroy(sht30_rp2040_t *sht30) {
	free(sht30);
}

bool sht30_rp2040_init(sht30_rp2040_t *sht30, struct sht30_rp2040_config_s config) {
	if (!sht30) return false; // Return 0 if sht30 pointer is NULL

	sht30->i2c = config.i2c;
	sht30->address = config.address;
	sht30->pin_scl = config.pin_scl;
	sht30->pin_sda = config.pin_sda;

	return true;
}

bool sht30_rp2040_read(sht30_rp2040_t *sht30, struct sht30_rp2040_reading_s *reading) {
	if (!sht30 || !reading) return false;

	uint8_t read_buffer[SHT30_READING_SIZE] = {0x00};

	int rval = i2c_write_blocking(sht30->i2c, sht30->address, 
			sht30_command_lookup[SHT30_SSDA_CS_HIGH], SHT30_COMMAND_SIZE, false);
	if (rval != SHT30_COMMAND_SIZE) return false;
	
	rval = i2c_read_blocking(sht30->i2c, sht30->address, read_buffer, 
			SHT30_READING_SIZE, false);
	if (rval != SHT30_READING_SIZE) return false;

	reading->temperature = 
		-45 + (175 * (read_buffer[0] << 8 | read_buffer[1])/ 65535.0);
	reading->humidity = 
		100 * (read_buffer[3] << 8 | read_buffer[4]) / 65535.0;
	
	return true;
}
