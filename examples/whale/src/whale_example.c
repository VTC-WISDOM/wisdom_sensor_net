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
	while (!tud_cdc_connected()) { sleep_ms(100); };
	printf("meow\n");


	//whale init! with every module
	int modules = 0x0F;
	int status = whale_init(modules);
	if(status != WHALE_OK) goto ERROR_LOOP;

	w_radio_node_address_set(NODE_ADDR);
	uint8_t tx_buffer[] = "meow!";

	w_radio_dbm_set(20);

	if(w_radio_tx(DEST_NODE, tx_buffer, sizeof(tx_buffer) != W_RADIO_OK)) {
		printf("tx failed :(\n");
	} else printf("tx success!\n");


	float bat;
	struct w_rtc_datetime_t datetime;
	uint32_t eeprom_pointer = 0;
	uint16_t page_pointer;
	uint8_t addr_pointer;
	uint8_t buffer[] = "xx-xx-xxTxx:xx:xxVx.xx";

	for(;;) {	
		w_rtc_datetime_get(&datetime);
		printf("20%02u-%02u-%02uT%02u:%02u:%02u\n",
				datetime.years, datetime.months, datetime.days,
				datetime.hours, datetime.minutes, datetime.seconds);

		w_get_battery_volts(&bat);
		printf("battery volts: %4.2f\n", bat);
		sprintf(buffer, "%02u-%02u-%02uT%02u:%02u:%02uV%4.2f", 
				datetime.years, datetime.months, datetime.days,
				datetime.hours, datetime.minutes, datetime.seconds,
				bat);

		eeprom_pointer += sizeof(buffer);
		addr_pointer = eeprom_pointer & 0xFF;
		page_pointer = eeprom_pointer >> 8;

		if(addr_pointer > (256 - sizeof(buffer))) {
			eeprom_pointer += 0x100;
			eeprom_pointer &= 0x00FFFF00;
			addr_pointer = eeprom_pointer & 0xFF;
			page_pointer = eeprom_pointer >> 8;
		}

		status = w_eeprom_write(page_pointer, addr_pointer, buffer, sizeof(buffer));
		if(status != W_EEPROM_OK) printf("failed to write to eeprom\n");
		else {
			w_eeprom_read(page_pointer, addr_pointer, buffer, sizeof(buffer));
			printf("wrote to eeprom page %u address %u: \n%s\n", page_pointer, addr_pointer, buffer);
		}


		printf("\n");

		sleep_ms(2000);
	}

ERROR_LOOP:
	for(;;) {
		printf("error!");
		sleep_ms(500);
	}

}
