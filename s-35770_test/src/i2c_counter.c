// i2c_counter.c

//	Copyright (C) 2024 
//	Evan Morse
//	Amelia Vlahogiannis
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
#include "hardware/i2c.h"
#include "tusb.h"

int main() {
    stdio_init_all(); // To be able to use printf

	i2c_init(i2c0, 100 * 1000);

	gpio_set_function(0, GPIO_FUNC_I2C);//pin 1 SDA
	gpio_set_function(1, GPIO_FUNC_I2C); //pin 2 SCL
	//gpio_set_function(3, GPIO_FUNC_SIO); 
	//gpio_pull_down(3); 
	//gpio_put(3, 0); 

	gpio_set_function(2, GPIO_FUNC_SIO); // reset pin, low to clear, keep high otherwise
	gpio_pull_up(2); //internal pullup on reset pin
	gpio_put(2, 0); //initial count reset
	sleep_ms(1000); 
	gpio_put(2, 1); //drive reset pin high, to note lose count

	gpio_pull_up(0); //I2C line pull ups
	gpio_pull_up(1);

	int i = 1;
	for(;;) {
		// Wait for USB serial connection
		while (!tud_cdc_connected()) { sleep_ms(100); };

#define buf_size 3

		uint8_t buf[buf_size] = {0};
		int Data_Return = i2c_read_blocking(i2c0, 0x32, buf, buf_size, false); 

		printf("i2c_read_return_value: %i\n", Data_Return); //check i2c function for return


		for (int i = 0; i < buf_size; i++) {
			printf("%d: %02X\n", i, buf[i]);
		}

		sleep_ms(1000);
		//gpio_put(3, 1);
		//gpio_put(3, 0);
		//sleep_ms(1000);

	}
    
    return 0;
}
