// radio_rx.c

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

void main() {
	stdio_init_all();
	while (!tud_cdc_connected()) { sleep_ms(100); };

	printf("meow\n");
	if(w_rtc_init()) goto ERROR_LOOP;

	struct w_rtc_datetime_t datetime;


	for(;;) {
		w_rtc_datetime_get(&datetime);
		printf("%02u:%02u:%02u\n", datetime.hours, datetime.minutes, datetime.seconds);
		sleep_ms(1000);
	}

ERROR_LOOP:
	for(;;) {
		printf("error!");
		sleep_ms(500);
	}

}
