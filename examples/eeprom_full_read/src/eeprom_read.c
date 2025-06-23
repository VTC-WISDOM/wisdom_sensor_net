// whale_eeprom.c

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
#include "hardware/i2c.h"


#define I2C_INST (i2c0)
#define PIN_SCL  (5)
#define PIN_SDA  (4)


void main() {
	stdio_init_all();

	while (!tud_cdc_connected()) { sleep_ms(100); };

	w_eeprom_init();

	int status = -1;

	uint8_t buffer[256];

	printf("contents of eeprom:\n\n");

	for(int i = 0; i < 512; i++) {
		status = w_eeprom_read(i, 0x00, buffer, sizeof(buffer));
		if(status == W_EEPROM_OK) {
			for(int o = 0; o < 256; o++) {
				if(buffer[o] == 0xFF || buffer == 0x00) break;
				putchar(buffer[o]);
			}
			printf("\n");
	}
		else printf("couldn't read page %i", i);

	}

	for(;;) {
		sleep_ms(1000);
	}

ERROR_LOOP:
	for(;;) {
		printf("error!");
		sleep_ms(500);
	}

}
