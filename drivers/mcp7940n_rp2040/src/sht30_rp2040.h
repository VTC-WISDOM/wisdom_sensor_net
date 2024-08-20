#ifndef SHT30_RP2040_WISDOM_H
#define SHT30_RP2040_WISDOM_H

#include <stdbool.h>

#include "hardware/i2c.h"

#define SHT30_READING_SIZE 6
#define SHT30_COMMAND_SIZE 2

typedef unsigned uint;

typedef struct _sht30_rp2040_s sht30_rp2040_t;
struct sht30_rp2040_reading_s {
	float temperature;
	float humidity;
};

struct sht30_rp2040_config_s {
	i2c_inst_t *i2c;
	uint address;
	uint pin_scl;
	uint pin_sda;
};

sht30_rp2040_t * sht30_rp2040_create(void);
void sht30_rp2040_destroy(sht30_rp2040_t *sht30);

// Since i2c is a shared protocol, i2c should be initialized BEFORE
// passing to sht30 init. 
bool sht30_rp2040_init(
		sht30_rp2040_t *sht30, struct sht30_rp2040_config_s);
bool sht30_rp2040_read(
		sht30_rp2040_t *sht30, struct sht30_rp2040_reading_s *reading);

#endif //SHT30_RP2040_WISDOM_H
