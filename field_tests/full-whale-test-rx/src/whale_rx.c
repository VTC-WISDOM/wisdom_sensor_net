// whale_rx.c

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

void main() {
	stdio_init_all();
	//while (!tud_cdc_connected()) { sleep_ms(100); };

	//whale init! with every module
	int modules = 0x0F;
	int status = whale_init(modules);
	if(status != WHALE_OK) goto ERROR_LOOP;

	//set radio address and power level (6dBm = 4 mW)
	w_radio_node_address_set(NODE_ADDR);
	w_radio_dbm_set(6);

	struct w_rtc_datetime_t datetime; //date&time
	uint32_t eeprom_pointer = 0x00; //memory location - 8 LSBs for address, next 9 up for page
	uint16_t page_pointer; //to extract the page from the eeprom pointer
	uint8_t addr_pointer; //and the address

	uint8_t pointer_buffer[4]; //so that we can read the current actual location

	//read the next address pointer located at page 0 addr 0
	//this way we can write the next address pointer every time we write data
	//and if the device restarts, it remembers where to restart
	//if the eeprom is erased(to 0xFF) then just set it to page 1 addr 0
	w_eeprom_read(0x00, 0x00, pointer_buffer, 4);
	eeprom_pointer = pointer_buffer[4];
	eeprom_pointer += (pointer_buffer[3] << 8);
	eeprom_pointer += (pointer_buffer[2] << 16);
	printf("eeprom pointer: %x", eeprom_pointer);
	if(pointer_buffer[4] == 0xFF) eeprom_pointer = 0x00000100;	




	uint8_t buffer[36]; //packet buffer!
			    //in csv format! we will fill with yy-mm-dd,hh:mm:ss,olts,olts,rssi
	uint8_t rx_buffer[8];
	int rx_size;
	int tx_addr;
	int rssi;
	float bat;

	for(;;) {
		if(w_radio_rx(rx_buffer, sizeof(rx_buffer), &rx_size, &tx_addr) == W_RADIO_OK) {
			printf("rx success\n");
			//separate the address and page from the eeprom pointer
			//yea this is a complicated system and i dont like it and now i want to change it
			addr_pointer = eeprom_pointer & 0xFF;
			page_pointer = eeprom_pointer >> 8;

			//if we are going to run out of space on this page, just increment it to the next one
			if(addr_pointer > (255 - sizeof(buffer))) {
				eeprom_pointer += 0x100;
				eeprom_pointer &= 0x00FFFF00;
				addr_pointer = eeprom_pointer & 0xFF;
				page_pointer = eeprom_pointer >> 8;
			}
			//to exit everything when we run out of eeprom space
			if(page_pointer > 511) goto ERROR_LOOP;

			w_radio_rssi_get(&rssi);
			w_rtc_datetime_get(&datetime);
			w_get_battery_volts(&bat);
			sprintf(buffer, "%02u-%02u-%02u,%02u:%02u:%02u,%4.2f,%s,%i\n", 
					datetime.years, datetime.months, datetime.days,
					datetime.hours, datetime.minutes, datetime.seconds,
					bat,rx_buffer, rssi);

			//now write that data to the eeprom!
			w_eeprom_write(page_pointer, addr_pointer, buffer, sizeof(buffer));

			//and finally increment the pointer for next time and write it
			eeprom_pointer += sizeof(buffer);
			pointer_buffer[1] = eeprom_pointer >> 16;
			pointer_buffer[2] = eeprom_pointer >> 8;
			pointer_buffer[3] = eeprom_pointer;
			w_eeprom_write(0x00, 0x00, pointer_buffer, 4);

		} else printf("rx failed!\n");

	}

ERROR_LOOP:
	for(;;) {
		printf("error!");
		sleep_ms(500);
	}

}
