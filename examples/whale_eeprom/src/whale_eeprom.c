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
#include "hardware/i2c.h"


#define I2C_INST (i2c0)
#define PIN_SCL  (5)
#define PIN_SDA  (4)


void main() {
	stdio_init_all();

	while (!tud_cdc_connected()) { sleep_ms(100); };
	i2c_init(I2C_INST, 100 * 1000);
	gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
	gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
	gpio_pull_up(PIN_SCL);
	gpio_pull_up(PIN_SDA);
	uint index = I2C_NUM(I2C_INST);


	printf("meow\n");

	printf("write 0x45 to page 1 addr 1... ");
	if(cat24_write_byte(index, 0x00, 0x01, 0x01, 0x45)) printf("success!\n");
	else printf("fail :(\n");

	//uint8_t tmp;
	//while(!cat24_read_byte(index, 0x00, 0x00, 0x00, &tmp));
	sleep_ms(3);

	uint8_t buf = 0;	
	printf("read byte from page 1 addr 1 into buf... ");
	if(cat24_read_byte(index, 0x00, 0x01, 0x01, &buf)) {
		printf("success!\n");

		printf("read: 0x%x\n", buf);
	} else printf("fail :(\n");

	while(cat24_is_busy(index, 0x00));

	printf("write string to page 2 addr 0... ");
	uint8_t test_string[] = "meow!";
	if(cat24_write_page(index, 0x00, 0x02, 0x00, test_string, sizeof(test_string))) printf("success!\n");
	else printf("fail :(\n");

	while(cat24_is_busy(index, 0x00));

	printf("read back string from page 2 addr 0... ");
	uint8_t read_str[6];
	if(cat24_read_page(index, 0x00, 0x02, 0x00, read_str, sizeof(read_str))) {
		printf("success!\n");

		printf("read: %s\n", read_str);
	} else printf("fail :(\n");



	for(;;) {
		sleep_ms(1000);
	}

ERROR_LOOP:
	for(;;) {
		printf("error!");
		sleep_ms(500);
	}

}
