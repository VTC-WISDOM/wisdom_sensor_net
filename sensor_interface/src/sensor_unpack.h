#ifndef WISDOM_SENSOR_UNPACK_H
#define WISDOM_SENSOR_UNPACK_H

#include "sensor_interface.h"
#include "sensor_type.h"

// SENSOR FUNCTION HEADERS
#include "sensor_headers.h"

struct _sensor_generic *
(*sensor_unpack_table[SENSOR_TYPE_MAX])(uint8_t *buffer) = {
	[TEST] = &test_sensor_unpack,
	[SHT30] = &sht30_wsi_unpack
};

#endif // WISDOM_SENSOR_UNPACK_H
