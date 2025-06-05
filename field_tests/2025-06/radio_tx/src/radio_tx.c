// radio_tx.c

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

/*
 * 
 *
 */


#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "pico/stdlib.h"
#include "tusb.h"

#include "whale.h"
#include "pcf8523_rp2040.h"

#define RTC_SDA
#define RTC_SCL

void main() {
	stdio_init_all();
	//while (!tud_cdc_connected()) { sleep_ms(100); };
	
	struct pcf8523_time_date_s rtc_datetime;
	pcf8523_time_date_get_all(,&rtc_datetime);

	sleep_ms(2000);
	printf("meow");


	int rval = whale_init(W_RADIO_MODULE);
	if (rval != WHALE_OK)
		goto ERROR_LOOP;

	w_radio_node_address_set(0x01);

#define PAYLOAD_SIZE (64)
	uint8_t payload[PAYLOAD_SIZE] = "00:00,0.00,FFFF";
	for (int i = 0; i < PAYLOAD_SIZE; i++)
		payload[i] = i;

	uint8_t success[] = "tx success";
uint8_t failure[] = "tx failure";

	w_radio_dbm_set(20);

	for (;;) {

		if (w_radio_tx(0x02, payload, PAYLOAD_SIZE) != W_RADIO_OK) {
			printf("Tx failure\n");
		}
		else {
			printf("Tx success\n");
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
