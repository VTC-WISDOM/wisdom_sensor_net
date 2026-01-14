#ifndef WISDOM_SENSORS_H
#define WISDOM_SENSORS_H

#include <stdint.h>
#include <stdbool.h>

#include "sensor_headers.h"

typedef struct _sensor_generic sensor_t;
typedef unsigned uint;

// Takes a sensor_t generic pointer and packs a buffer with up to
// buffer_len bytes of sensor data. If data was packed successfully, returns
// number of bytes packed. Returns 0 if buffer is too small. -1 if
// data or buffer is NULL.
int sensor_pack(sensor_t *sensor, uint8_t *buffer, uint buffer_len);

bool sensor_read(sensor_t *sensor);

#endif // WISDOM_SENSORS_H
