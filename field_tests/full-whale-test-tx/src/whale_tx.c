// whale_tx.c

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

#define NODE_ADDR (0x02)
#define DEST_ADDR (0x01)

void main() {
	stdio_init_all();
	//while (!tud_cdc_connected()) { sleep_ms(100); };


	//whale init! with every module
	int modules = 0x0F;
	int status = whale_init(modules);
	if(status != WHALE_OK) goto ERROR_LOOP;

	//set radio address and power level
	w_radio_node_address_set(NODE_ADDR);
	w_radio_dbm_set(6);

	//battery voltage!
	float bat;
	uint8_t tx_buffer[8];
	
	//timekeeping
	struct w_rtc_datetime_t datetime;
	uint8_t minutes;

	for(;;) {

		//grab and store this time
		w_rtc_datetime_get(&datetime);
		minutes = datetime.minutes;
		
		//grab the battery voltage and transmit it
		w_get_battery_volts(&bat);
		sprintf(tx_buffer, "%4.2f", bat);
		w_radio_tx(DEST_ADDR, tx_buffer, 8);

		//wait for a minute to pass!
		while(datetime.minutes == minutes) {
			w_rtc_datetime_get(&datetime);
			sleep_ms(900);
		}

	}

ERROR_LOOP:
	for(;;) {
		printf("error!");
		sleep_ms(500);
	}

}
