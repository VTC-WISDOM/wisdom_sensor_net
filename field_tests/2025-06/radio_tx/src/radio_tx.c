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
#include <stdint.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "tusb.h"

#include "whale.h"
#include "pcf8523_rp2040.h"

#define I2C_INST (i2c0)
#define PIN_SCL  (5)
#define PIN_SDA  (4)
#define PIN_IRQ  (2)


void main() {
	stdio_init_all();
	//while (!tud_cdc_connected()) { sleep_ms(100); };

	//init i2c for rtc
	i2c_init(I2C_INST, 500 * 1000);
	gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
	gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
	gpio_pull_up(PIN_SCL);
	gpio_pull_up(PIN_SDA);
	gpio_pull_up(PIN_IRQ);
	uint index = I2C_NUM(I2C_INST);

	//rtc init
	if(!pcf8523_software_reset_initiate(index)) goto ERROR_LOOP;
	bool timecheck;
	pcf8523_time_circuit_is_running(index, &timecheck);
	if(!timecheck) {
		printf("time circuit not running!\n");
		goto ERROR_LOOP;
	}

	for(;;) {
	uint8_t seconds;
	pcf8523_seconds_get(index, &seconds);	
	uint8_t minutes;
	pcf8523_minutes_get(index, &minutes);	
	uint8_t hours;
	pcf8523_hours_get(index, &hours);
	uint8_t years;
	pcf8523_years_get(index, &years);
	uint8_t months;
	pcf8523_months_get(index, &months);
	uint8_t days;
	pcf8523_days_get(index, &days);
	

	
	//for serial monitor purposes
	sleep_ms(2000);
	printf("%02u-%02u-%02uT%02u:%02u:%02u\n", 
			years, months, days,
			hours, minutes, seconds);
	}

	//radio init
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
		printf("Error!");
		printf("%u\n", w_module_state_query(W_RADIO_MODULE));
		sleep_ms(3000);
	}
}
