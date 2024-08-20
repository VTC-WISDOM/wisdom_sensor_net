#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "gateway_interface.h"
#include "gateway_queue.h"
#include "gateway_error.h"
#include "gw_core.h"
#include "gw_core_error.h"

#include "gateway_codes.h" // Communication codes


void gateway_init(void) {
	gw_queue_init();

	multicore_launch_core1(gw_core_entry);
}

void gateway_start(void) {

	uint32_t command[2];
	command[0] = GATEWAY_COMMAND;
	command[1] = GATEWAY_START;
	gw_queue_main_send((uint8_t *)command, (sizeof (uint32_t)) * 2);

}

void gateway_stop(void) {
	uint32_t command[2];
	command[0] = GATEWAY_COMMAND;
	command[1] = GATEWAY_STOP;
	gw_queue_main_send((uint8_t *)command, (sizeof (uint32_t)) * 2);
}

void gateway_send(void *data, uint size) {
	uint32_t header = PACKED_DATA; 	

	gw_queue_main_send((uint8_t *)&header, (sizeof (uint32_t)));
	gw_queue_main_send((uint8_t *)data, size);

}
