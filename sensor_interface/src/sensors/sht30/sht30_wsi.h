#ifndef SHT30_WSI_H
#define SHT30_WSI_H

#include "sht30_rp2040.h"
#include "sensor_interface.h"

typdef struct _sht30_wsi_s {
	struct _sensor_generic header;
	sht30_rp2040_t *reading;
} sht30_wsi_t;

sht30_wsi_t * sht30_wsi_create(void);
void * sht30_wsi_destroy(sht30_wsi_t *sensor);

int sht30_wsi_pack(
		struct _sensor_generic *sensor,
		uint8_t *buffer, 
		uint buffer_len
);

bool sht30_wsi_read(struct _sensor_generic *sensor);
struct _sensor_generic * sht30_wsi_unpack(uint8_t *buffer);

#endif // SHT30_WSI_H
