// whale_radio.c

//	Copyright (C) 2024 
//	Evan Morse
//	Amelia Vlahogiannis
//	Sam Cowan
//	Marin Bove

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

#include "pico/stdlib.h"
#include "tusb.h"

#include "whale.h"

#define NODE_ADDR (0x01)
#define DEST_NODE (0x02)

void main() {
	stdio_init_all();
	//while (!tud_cdc_connected()) { sleep_ms(100); };
	printf("meow\n");

	//whale init! with the radio module
	int modules = 0 | W_RADIO_MODULE;
	int status = whale_init(modules);
	if(status != WHALE_OK) goto ERROR_LOOP;

	w_radio_node_address_set(NODE_ADDR);
	uint8_t tx_buffer[] = "meow!";

	w_radio_dbm_set(20);

	if(w_radio_tx(DEST_NODE, tx_buffer, sizeof(tx_buffer) != W_RADIO_OK)) {
		printf("tx failed :(\n");
	} else printf("tx success!\n");


	int rx_size = 0;
	int tx_addr = 0;
	int rssi = 0;
	uint8_t rx_buffer[64];


	for(;;) {	

		if(w_radio_rx(rx_buffer, sizeof(rx_buffer), &rx_size, &tx_addr) != W_RADIO_OK) {
			printf("rx failed :(\n");
		} else {

			w_radio_rssi_get(&rssi);
			printf("received %i bytes from address %i:\n%s\n%i",
					rx_size, tx_addr, rx_buffer, rssi);
		}

			sleep_ms(1000);
		}

ERROR_LOOP:
		for(;;) {
			printf("error!");
			sleep_ms(500);
		}

	}
