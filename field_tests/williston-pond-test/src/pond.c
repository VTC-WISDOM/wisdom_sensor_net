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
#include "hardware/i2c.h"

#include "whale.h"
#include "sht30_rp2040.h"

#include <inttypes.h>
#include <stdbool.h>
#include "pico/critical_section.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"

#define trig 28
#define echo 29
#define delay 10
#define propogation_velocity 340
#define measurement_cycle 60

#define ever ;;


volatile uint64_t re_ts = {0};
volatile double distance_mm = {0};
volatile bool edge_fall = false;

void gpio_callback(uint gpio, uint32_t events) {

  if (events & GPIO_IRQ_EDGE_RISE) {
    re_ts = to_us_since_boot(get_absolute_time());
  }
  
  else if (events & GPIO_IRQ_EDGE_FALL) {   
    uint64_t fe_ts = to_us_since_boot(get_absolute_time());
    double delta_t_us = (fe_ts - re_ts);

    distance_mm = (((delta_t_us/1000000)*propogation_velocity)/2)*1000;
    edge_fall = true;
  }

}

void main() {
	stdio_init_all();
	//while (!tud_cdc_connected()) { sleep_ms(100); };

	gpio_init(trig);
  gpio_pull_down(trig);
  gpio_set_dir(trig, GPIO_OUT);
  gpio_put(trig, 0);
  gpio_init(echo);
  gpio_set_irq_enabled_with_callback(echo, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, gpio_callback);


	//whale init! with every module
	int modules = 0x0C;
	int status = whale_init(modules);
	if(status != WHALE_OK) goto ERROR_LOOP;

	struct w_rtc_datetime_t datetime; //date&time
	uint32_t eeprom_pointer = 0x00000000; //memory location - 8 LSBs for address, next 9 up for page
	uint16_t page_pointer; //to extract the page from the eeprom pointer
	uint8_t addr_pointer; //and the address

	uint8_t pointer_buffer[3] = {0}; //so that we can read the current actual location

	//read the next address pointer located at page 0 addr 0
	//this way we can write the next address pointer every time we write data
	//and if the device restarts, it remembers where to restart
	//if the eeprom is erased(to 0xFF) then just set it to page 1 addr 0
	w_eeprom_read(0x00, 0x00, pointer_buffer, 4);
	eeprom_pointer = pointer_buffer[2];
	eeprom_pointer += ((uint32_t)pointer_buffer[1]) << 8;
	eeprom_pointer += ((uint32_t)pointer_buffer[0]) << 16;
	if(pointer_buffer[0] == 0xFF) eeprom_pointer = 0x00000100;
	printf("eeprom pointer: %08x\n", eeprom_pointer);


	uint8_t buffer[64]; //to write to the eeprom
	float bat;
	uint8_t prev_minutes = 0;


	struct sht30_reading_s sht;
	
	for(ever) {

						
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


		gpio_put(trig,1);
		busy_wait_us(delay);
    gpio_put(trig,0);
    for(int i = 0; i < 60 && !edge_fall; i++) {
    	sleep_ms(1);}
    edge_fall = false;
    //recommended 60ms measurement cycle time
    busy_wait_ms(measurement_cycle);
    //end us sensor code

    
			w_rtc_datetime_get(&datetime);
			w_get_battery_volts(&bat);
			sht30_rp2040_read(i2c_get_index(i2c0), &sht);
			sprintf(buffer, "%02u-%02u-%02u,%02u:%02u,%4.2f,%04.1f,%04.1f, %04.0f\n", 
					datetime.years, datetime.months, datetime.days,
					datetime.hours, datetime.minutes, bat,
					sht.temperature, sht.humidity, distance_mm);

			printf("eeprom pointer: %u\n", eeprom_pointer);
			printf("%s", buffer);

			//now write that data to the eeprom!
			w_eeprom_write(page_pointer, addr_pointer, buffer, sizeof(buffer));

			//and finally increment the pointer for next time and write it
			eeprom_pointer += sizeof(buffer);
			pointer_buffer[0] = eeprom_pointer >> 16;
			pointer_buffer[1] = eeprom_pointer >> 8;
			pointer_buffer[2] = eeprom_pointer;
			w_eeprom_write(0x00, 0x00, pointer_buffer, sizeof(pointer_buffer));


		//wait for the minutes to change, 5 times
		for(int i = 0; i < 5; i++) {
			//grab time now
				w_rtc_datetime_get(&datetime);
				//store it
				prev_minutes = datetime.minutes;
				//now get the time once every (n) ms until the minutes change
				do {
					w_rtc_datetime_get(&datetime);
					sleep_ms(10000);
					} while(datetime.minutes == prev_minutes);
				}//for
			}//for(ever)

	
ERROR_LOOP:
	for(ever) {
		printf("error!");
		sleep_ms(500);
	}

}
