#include <stddef.h>

#include "sensor_interface.h"

void sensor_data_init(
	struct _sensor_generic *sd,
	SENSOR_TYPE_T type,
	int (*pack_func)(struct _sensor_generic *, uint8_t *, uint),
	bool (*read_func)(struct _sensor_generic *)
)
{
	sd->type = type;	
	sd->pack_func = pack_func;
	sd->read_func = read_func;
}
