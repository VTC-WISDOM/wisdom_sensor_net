// template_main.c

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
#include "pico/stdlib.h"
#include "tusb.h"

// OPTIONAL INCLUDES
// Uncomment depending on libraries used

// RFM69 library
//#include "rfm69_pico.h" 

// SD card library
//#include "sd_config.h"

// EEPROM Library
#include "cat24m01_rp2040.h"

#define PIN_SCL 5
#define PIN_SDA 4
#define I2C_INST (i2c0)

cat24_inst_t eeprom = {
	.a1 = 0,
	.a2 = 0,
	.i2c = I2C_INST,
	.pin_scl = PIN_SCL,
	.pin_sda = PIN_SDA,
};

int main() {
	stdio_init_all(); // To be able to use printf
	i2c_init(I2C_INST, 500 * 1000);
	gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
	gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
	gpio_set_pulls(PIN_SCL, 1, 0);
	gpio_set_pulls(PIN_SDA, 1, 0);

	uint8_t data = 0x45;
	uint16_t page = 420;
	uint8_t address = 69;
	uint8_t rxbuf = 0xaa;



	while (!tud_cdc_connected()) { sleep_ms(100); };
	sleep_ms(100);

	printf("usb connected\n");

	bool status = cat24_write_byte(&eeprom, page, address, data);
	if(!status) printf("wrote to eeprom\n");
	else printf("failed write!\n");

	sleep_ms(100);

	status = cat24_read_selective_byte(&eeprom, page, address, &rxbuf);
	if(!status) printf("read byte from eeprom\n");
	else printf("failed read!\n");

	sleep_ms(100);

	printf("read byte %i from page %i: 0x%x\n", address, page, rxbuf);


	return 0;
}
