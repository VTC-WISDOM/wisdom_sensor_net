#ifndef TEST_SENSOR_H
#define TEST_SENSOR_H

#include "sensor_interface.h"

typedef struct _test_sensor {
	struct _sensor_generic header;
	uint reading;
} test_sensor_t;

test_sensor_t * test_sensor_create(void);
void * test_sensor_destroy(test_sensor_t *sensor);

int test_sensor_pack(
		struct _sensor_generic *sensor, 
		uint8_t *buffer, 
		uint buffer_len
);
bool test_sensor_read(struct _sensor_generic *sensor);
struct _sensor_generic * test_sensor_unpack(uint8_t *buffer);

#endif // TEST_SENSOR_H
