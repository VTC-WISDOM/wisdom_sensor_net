// radio_rx.c

//	Copyright (C) 2024 
//	Evan Morse
//	Amelia Vlahogiannis
//	Noelle Steil
//	Jordan Allen
//	Sam Cowan
//	Rachel Cleminson

//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.

//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.

//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "pico/stdlib.h"
#include "tusb.h"

#include "whale.h"

#define MONITOR_ADDR 0x45

void main() {
	stdio_init_all();
	while (!tud_cdc_connected()) { sleep_ms(100); };
	
	int rval = whale_init(W_RADIO_MODULE);
	if (rval != WHALE_OK)
		goto ERROR_LOOP;

	w_radio_node_address_set(0x02);

#define PAYLOAD_BUFFER_SIZE (64)
	uint8_t buffer[PAYLOAD_BUFFER_SIZE] = {0};
	int received = {0};
	int tx_addr = {0};


	int rssi_raw = 0;
	//uint8_t rssi[8];
	
	w_radio_dbm_set(20);
	printf("time(UTC),battery(V),rssi(dB)\n");
	for (;;) {

		if (w_radio_rx(buffer, PAYLOAD_BUFFER_SIZE, &received, &tx_addr) != W_RADIO_OK) {
			//w_radio_tx(MONITOR_ADDR, failure, sizeof(failure));
			printf("Rx failed: %i\n", w_radio_error_get());
			}
		else {

			w_radio_rssi_get(&rssi_raw);
			printf("%s,%i\n", buffer, rssi_raw);
		}

		sleep_ms(1000);
	}

	// Loop forever with error
ERROR_LOOP:
	for (;;) {
		printf("Radio module failed to initialize: ");
		printf("%u\n", w_module_state_query(W_RADIO_MODULE));
		sleep_ms(3000);
	}
}
