#ifndef SHT30_WSI_H
#define SHT30_WSI_H

#include "sht30_rp2040.h"
#include "sensor_interface.h"

typedef struct _sht30_wsi_s {
	struct _sensor_generic header;
	uint i2c_index;
	struct sht30_reading_s reading;
} sht30_wsi_t;

void sht30_wsi_init(sht30_wsi_t *sensor, uint i2c_index);

int sht30_wsi_pack(
		struct _sensor_generic *sensor,
		uint8_t *buffer, 
		uint buffer_len
);

bool sht30_wsi_read(struct _sensor_generic *sensor);

#endif // SHT30_WSI_H
