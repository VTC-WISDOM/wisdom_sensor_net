// template_main.c

//	Copyright (C) 2024 
//	Evan Morse
//	Amelia Vlahogiannis
//	Noelle Steil
//	Jordan Allen
//	Sam Colwell
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
#include "hardware/i2c.h"
#include "tusb.h"

// OPTIONAL INCLUDES
// Uncomment depending on libraries used

// RFM69 library
//#include "rfm69_pico.h" 

// SD card library
//#include "sd_config.h"

#define I2C_INST (i2c0)
#define I2C_ADDRESS (0x68)
#define I2C_BAUD (100 * 2000)
#define PIN_SCL (9)
#define PIN_SDA (8)

int main() {
	// To be able to use printf
    stdio_init_all(); 

	// Wait for USB serial connection
	while (!tud_cdc_connected()) { sleep_ms(100); };

	gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
	gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);

	uint baud = i2c_init(I2C_INST, I2C_BAUD);
	if (baud != I2C_BAUD) {
		printf("i2c_init failed to set desired baud\n");
		printf("desired: %u\n", I2C_BAUD);
	}
	printf("i2c baud set: %u\n", baud);

	uint8_t buf[2] = {0x03, 0x00};
	uint rval = i2c_write_blocking(I2C_INST, I2C_ADDRESS, buf, 2, false);

	int i = 1;
	for(;;) {
		
		buf[0] = 0x03;
		uint rval = i2c_write_blocking(I2C_INST, I2C_ADDRESS, buf, 1, false);
		if (rval == PICO_ERROR_GENERIC) printf("%02X: unable to write\n", i);
		rval = i2c_read_blocking(I2C_INST, I2C_ADDRESS, buf, 2, false);
		if (rval == PICO_ERROR_GENERIC) printf("%02X: unable to read\n", i);
		printf("rval: %02X\n", rval);

		printf("min: %02X\n", buf[1]);
		printf("sec: %02X\n", buf[0]);
		
		i++;
		sleep_ms(1000);
	}
    
    return 0;
}
