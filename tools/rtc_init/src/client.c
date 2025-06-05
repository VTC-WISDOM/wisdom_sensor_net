// client.c

//	Copyright (C) 2024 
//	Evan Morse
//	Amelia Vlahogiannis
//	Noelle Steil
//	Jordan Allen
//	Sam Cowen
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
#include <string.h>
#include <time.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "tusb.h"

#include "pcf8523_rp2040.h"
#include "rtc_sync.h"

#define I2C_INST (i2c0)
#define PIN_SCL  (5)
#define PIN_SDA  (4)
#define PIN_IRQ  (2)

const char *weekday_string_lookup[] = {
	[SUNDAY]	= "Sunday",
	[MONDAY]	= "Monday",
	[TUESDAY]	= "Tuesday",
	[WEDNESDAY] = "Wednesday",
	[THURSDAY]	= "Thursday",
	[FRIDAY]	= "Friday",
	[SATURDAY]	= "Saturday"
};

void error_loop(char *error) {
	for (;;) {
		printf("Error: %s\n", error);
		sleep_ms(3000);
	}
}

int main() {
    stdio_init_all(); // To be able to use printf
	
	bool success = false;
	

	i2c_init(I2C_INST, 500 * 1000);
	gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
	gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
	gpio_pull_up(PIN_SCL);
	gpio_pull_up(PIN_SDA);
	gpio_pull_up(PIN_IRQ);

	uint index = I2C_NUM(I2C_INST);

	if (!pcf8523_software_reset_initiate(index))
		goto LOOP_BEGIN;

	success = true;
LOOP_BEGIN:;

	while (!tud_cdc_connected()) { sleep_ms(100); };

	uint buf = RTC_SYNC_REQUEST;
	fwrite(&buf, sizeof buf, 1, stdout); 
	fflush(stdout);

	time_t rawtime;
	fread(&rawtime, sizeof rawtime, 1, stdin);
	struct tm *time = gmtime(&rawtime);

	pcf8523_time_circuit_stop(index);

	pcf8523_clockout_freq_set(index, CLOCKOUT_DISABLED);
	pcf8523_pm_function_set(index, PM_SO_STANDARD_BLD_DISABLED);
	pcf8523_hour_mode_set(index, HOUR_MODE_24);

	pcf8523_seconds_set(index, time->tm_sec);
	pcf8523_minutes_set(index, time->tm_min);
	pcf8523_hours_set(index, time->tm_hour);
	pcf8523_days_set(index, time->tm_mday);
	pcf8523_months_set(index, time->tm_mon + 1);
	pcf8523_weekdays_set(index, time->tm_wday);
	pcf8523_years_set(index, time->tm_year % 100);

	pcf8523_time_circuit_start(index);
	pcf8523_ci_warning_flag_clear(index);

	buf = RTC_SYNC_SUCCESS;
	fwrite(&buf, sizeof buf, 1, stdout); 
	fflush(stdout);

	//char *payload = "Hello, Receiver!";
	//uint payload_len = strlen(payload) + 1;

	for (;;) {
		while (!tud_cdc_connected()) { sleep_ms(100); };

		uint8_t seconds;
		pcf8523_seconds_get(index, &seconds);	
		uint8_t minutes;
		pcf8523_minutes_get(index, &minutes);	
		uint8_t hours;
		pcf8523_hours_get(index, &hours);	
		AM_PM_T am_pm;
		pcf8523_am_pm_get(index, &am_pm);

		// [HH:MM:SS] [AM|PM]
		printf("%02u:%02u:%02u", hours, minutes, seconds);
		printf(" %s\n", am_pm == AM ? "AM" : "PM");

		MONTH_T month;
		pcf8523_months_get(index, &month);

		WEEKDAY_T weekday;
		pcf8523_weekdays_get(index, &weekday);

		uint8_t days;
		pcf8523_days_get(index, &days);

		uint8_t years;
		pcf8523_years_get(index, &years);

		// [WEEKDAY] [MONTH] [DAY], [YEAR]
		printf("%s", weekday_string_lookup[weekday]);
		printf(" %s", month_string_lookup[month]);
		printf(" %u, %02u\n", days, years);

		printf("\n");
		sleep_ms(1000);
	}
    
    return 0;
}
