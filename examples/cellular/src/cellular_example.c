// cellular_example.c

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
#include "rp2x_sim7080g.h"

void main() {
	stdio_init_all();

	while (!tud_cdc_connected()) { sleep_ms(100); };

	//create struct and all for the cell module
	sim7080g_context_t *sim;
	sim = sim7080g_create();


	const char sim_apn[] = "iot.1nce.net";
	//init the module
	sim7080g_init(sim, sim_apn, uart0, 0, 1, 14);
	bool status;
	//startup
	printf("starting sim7080g... ");
	if(!sim7080g_start(sim)) {
		printf("error starting sim7080g\n");
		goto ERROR_LOOP;
	} else printf("successfully started sim7080g\n");

	//config
	printf("configuring sim7080g... ");
	if(!sim7080g_config(sim)) {
		printf("error configuring sim7080g\n");
		goto ERROR_LOOP;
	} else printf("successfully configured sim7080g\n");

	//check if we can talk to the sim card
	printf("checking sim card status... ");
	if(!sim7080g_sim_ready(sim)) {
		printf("sim error\n");
		goto ERROR_LOOP;
	} else printf("sim card ok\n");

	//activate a cell network
	for(int i = 1; i <= 10; i++) {
		printf("connecting to a cell network(%i)... ", i);
		if(!sim7080g_cn_activate(sim, 1)) {
			printf("could not activate cell network\n");
			if(i == 10) goto ERROR_LOOP;
			sleep_ms(500);
		} else break;
	}
	printf("cell network activated\n");

	for(;;) {

		if(!sim7080g_cn_is_active(sim)) {
			printf("cell network inactive - reconnecting... ");
			if(!sim7080g_cn_activate(sim, 1)) printf("failed\n");
			else printf("cell network activated\n");
		} else printf("cell network active\n");

		sleep_ms(5000);
	}

ERROR_LOOP:
	for(;;) {
		printf("error!");
		sleep_ms(500);
	}

}
