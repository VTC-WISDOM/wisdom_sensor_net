#include "stdlib.h"

#include "test_sensor.h"
#include "sensor_type.h"

#define TEST_SENSOR_PACKED_SIZE (sizeof (SENSOR_TYPE) + sizeof (uint))

test_sensor_t *test_sensor_create(void) {
	test_sensor_t *sensor = malloc(sizeof *sensor);
	if (sensor == NULL) return NULL;

	sensor_data_init(
			&sensor->header,
			TEST,
			&test_sensor_pack,
			&test_sensor_read
	);

	return sensor;
}

void *test_sensor_destroy(test_sensor_t *sensor) {
	free(sensor);
}

int test_sensor_pack(
		struct _sensor_generic *sensor, 
		uint8_t *buffer, 
		uint buffer_len
) 
{
	if (sensor == NULL || buffer == NULL) return -1;
	if (buffer_len < TEST_SENSOR_PACKED_SIZE) return 0;

	test_sensor_t *ts = (test_sensor_t *)sensor;

	*((SENSOR_TYPE *)buffer) = (SENSOR_TYPE)ts->header.type;
	buffer += sizeof (SENSOR_TYPE);
	*((uint *)buffer) = ts->reading;

	return TEST_SENSOR_PACKED_SIZE;
}

bool test_sensor_read(struct _sensor_generic *sensor) {
	if (sensor == NULL) return false;

	((test_sensor_t *)sensor)->reading = 5;

	return true;
}

// Server side
struct _sensor_generic * test_sensor_unpack(uint8_t *buffer) {
	test_sensor_t *sensor = test_sensor_create();
	if (sensor == NULL) return NULL;

	buffer += sizeof (SENSOR_TYPE);
	sensor->reading = *buffer;
	
	return (struct _sensor_generic *) sensor;
}
