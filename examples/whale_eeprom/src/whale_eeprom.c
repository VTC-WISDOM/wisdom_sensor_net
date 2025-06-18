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
#include "rp2x_cat24m01.h"


#define I2C_INST (i2c0)
#define PIN_SCL  (5)
#define PIN_SDA  (4)
#define PIN_IRQ  (2)


void main() {
	stdio_init_all();

	i2c_init(I2C_INST, 500 * 1000);
        gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
        gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
        gpio_pull_up(PIN_SCL);
        gpio_pull_up(PIN_SDA);
        gpio_pull_up(PIN_IRQ);
        uint index = I2C_NUM(I2C_INST);

	while (!tud_cdc_connected()) { sleep_ms(100); };

	printf("meow\n");

	cat24_write_byte(index, 0x00, 0x01, 0x01, 0x45);
	uint8_t buf = 0;
	cat24_read_byte(index, 0x00, 0x01, 0x01, &buf);

	printf("%u", buf);


	for(;;) {
		sleep_ms(1000);
	}

ERROR_LOOP:
	for(;;) {
		printf("error!");
		sleep_ms(500);
	}

}
