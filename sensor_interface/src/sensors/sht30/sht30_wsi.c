#include "sensors/sht30/sht30_wsi.h"

sht30_wsi_t * sht30_wsi_create(void) {
	sht30_wsi_t *sensor = malloc(sizeof *sensor);
	if (!sensor) return NULL;

	sensor->reading = malloc(sizeof *sensor->reading);
	if (!sensor->reading) {
		free(sensor);
		return NULL;
	}

	return sensor;		
}

void * sht30_wsi_destroy(sht30_wsi_t *sensor) {
	if (!sensor) return;
}

int sht30_wsi_pack(
		struct _sensor_generic *sensor,
		uint8_t *buffer, 
		uint buffer_len
)
{
}

bool sht30_wsi_read(struct _sensor_generic *sensor) {

}

struct _sensor_generic * sht30_wsi_unpack(uint8_t *buffer) {

}
